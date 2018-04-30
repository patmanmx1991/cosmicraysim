//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: DBEvaluator.cc 89243 2015-03-27 16:24:39Z gcosmo $
// GEANT4 tag $ Name:$
//
// class DBEvaluator Implementation
//
// Original author: Zoltan Torzsok, November 2007
//
// --------------------------------------------------------------------

#include <sstream>

#include "DBEvaluator.hh"
#include "G4SystemOfUnits.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"

namespace COSMIC {

DBEvaluator::DBEvaluator()
{
  eval.clear();
  eval.setStdMath();
  eval.setSystemOfUnits(meter, kilogram, second, ampere, kelvin, mole, candela);

  // Should register variables from inside the database here
  std::vector<DBTable*> dtbls = DB::Get()->GetLinkGroup("VARIABLE");

  for (uint i = 0; i < dtbls.size(); i++) {
    std::vector<std::string> fields = dtbls[i]->GetFields();
    for (uint j = 0; j < fields.size(); j++) {
      if (fields[j].compare("index")==0) continue;
      if (fields[j].compare("name")==0) continue;
      G4double dbval = Evaluate(dtbls[i]->GetS(fields[j]));
      DefineConstant(fields[j], dbval);
    }
  }
}

DBEvaluator::~DBEvaluator(){
  Clear();
}

void DBEvaluator::Clear()
{
  eval.clear();
  eval.setStdMath();
  eval.setSystemOfUnits(meter, kilogram, second, ampere, kelvin, mole, candela);

  variableList.clear();
}

void DBEvaluator::DefineConstant(const G4String& name, G4double value)
{
  if (eval.findVariable(name))
  {
    G4String error_msg = "Redefinition of constant or variable: " + name;
    G4Exception("DBEvaluator::DefineConstant()", "InvalidExpression",
                FatalException, error_msg);
  }
  eval.setVariable(name.c_str(), value);
}

void DBEvaluator::DefineVariable(const G4String& name, G4double value)
{
  if (eval.findVariable(name))
  {
    G4String error_msg = "Redefinition of constant or variable: " + name;
    G4Exception("DBEvaluator::DefineVariable()", "InvalidExpression",
                FatalException, error_msg);
  }
  eval.setVariable(name.c_str(), value);
  variableList.push_back(name);
}

void DBEvaluator::DefineMatrix(const G4String& name,
                               G4int coldim,
                               std::vector<G4double> valueList)
{
  const G4int size = valueList.size();

  if (size == 0)
  {
    G4String error_msg = "Matrix '" + name + "' is empty!";
    G4Exception("DBEvaluator::DefineMatrix()", "InvalidSize",
                FatalException, error_msg);
  }
  /*
  if (size == 1)
  {
    G4String error_msg = "Matrix '" + name
                       + "' has only one element! "
                       + "Define a constant instead!!";
    G4Exception("DBEvaluator::DefineMatrix()", "InvalidSize",
                FatalException, error_msg);
  }
  */

  if (size % coldim != 0)
  {
    G4String error_msg = "Matrix '" + name + "' is not filled correctly!";
    G4Exception("DBEvaluator::DefineMatrix()", "InvalidSize",
                FatalException, error_msg);
  }

  if ((size == coldim) || (coldim == 1))   // Row- or column matrix
  {
    for (G4int i = 0; i < size; i++)
    {
      std::stringstream MatrixElementNameStream;
      MatrixElementNameStream << name << "_" << i;
      DefineConstant(MatrixElementNameStream.str(), valueList[i]);
    }
  }
  else   // Normal matrix
  {
    const G4int rowdim = size / coldim;

    for (G4int i = 0; i < rowdim; i++)
    {
      for (G4int j = 0; j < coldim; j++)
      {
        std::stringstream MatrixElementNameStream;
        MatrixElementNameStream << name << "_" << i << "_" << j;
        DefineConstant(MatrixElementNameStream.str(), valueList[coldim * i + j]);
      }
    }
  }
}

void DBEvaluator::SetVariable(const G4String& name, G4double value)
{
  if (!IsVariable(name))
  {
    G4String error_msg = "Variable '" + name + "' is not defined!";
    G4Exception("DBEvaluator::SetVariable()", "InvalidSetup",
                FatalException, error_msg);
  }
  eval.setVariable(name.c_str(), value);
}

G4bool DBEvaluator::IsVariable(const G4String& name) const
{
  const size_t variableCount = variableList.size();

  for (size_t i = 0; i < variableCount; i++)
  {
    if (variableList[i] == name)  { return true; }
  }

  return false;
}

G4String DBEvaluator::SolveBrackets(const G4String& in)
{
  std::string::size_type full  = in.size();
  std::string::size_type open  = in.find("[", 0);
  std::string::size_type close = in.find("]", 0);

  if (open == close) { return in; }

  if ((open > close) || (open == std::string::npos) || (close == std::string::npos))
  {
    G4String error_msg = "Bracket mismatch: " + in;
    G4Exception("DBEvaluator::SolveBrackets()", "InvalidExpression",
                FatalException, error_msg);
    return in;
  }

  std::string::size_type begin = open;
  std::string::size_type end = 0;
  std::string::size_type end1 = 0;
  std::string out;
  out.append(in, 0, open);

  do  // Loop for all possible matrix elements in 'in'
  {
    do   // SolveBrackets for one matrix element
    {
      end = in.find(",", begin + 1);
      end1 = in.find("]", begin + 1);
      if (end > end1)                { end = end1; }
      if (end == std::string::npos)  { end = close;}

      std::stringstream indexStream;
      indexStream << "_" << EvaluateInteger(in.substr(begin + 1, end - begin - 1)) - 1;

      out.append(indexStream.str());

      begin = end;

    } while (end < close);

    if (full == close) { return out; }

    open  = in.find("[", begin);
    close = in.find("]", begin + 1);

    if (open == close) { out.append(in.substr(end + 1, full - end - 1)); return out; }
    out.append(in.substr(end + 1, open - end - 1));

    begin = open;

  } while (close < full);

  return out;
}

G4double DBEvaluator::Evaluate(const G4String& in)
{
  G4String expression = SolveBrackets(in);

  G4double value = 0.0;

  if (!expression.empty())
  {
    value = eval.evaluate(expression.c_str());

    if (eval.status() != G4Evaluator::OK)
    {
      eval.print_error();
      G4String error_msg = "Error in expression: " + expression;
      G4Exception("DBEvaluator::Evaluate()", "InvalidExpression",
                  FatalException, error_msg);
    }
  }
  return value;
}

G4int DBEvaluator::EvaluateInteger(const G4String& expression)
{
  // This function is for evaluating integer expressions,
  // like loop variables and matrix indices.
  // Complains if the evaluated expression has a fractional
  // part different from zero

  G4double value = Evaluate(expression);

  G4int whole = (G4int)value;
  G4double frac = value - (G4double)whole;

  if (frac != 0.0)
  {
    G4String error_msg = "Expression '" + expression
                         + "' is expected to have an integer value!";
    G4Exception("DBEvaluator::EvaluateInteger()", "InvalidExpression",
                FatalException, error_msg);
  }
  return whole;
}

G4double DBEvaluator::GetConstant(const G4String& name)
{
  if (IsVariable(name))
  {
    G4String error_msg = "Constant '" + name
                         + "' is not defined! It is a variable!";
    G4Exception("DBEvaluator::GetConstant()", "InvalidSetup",
                FatalException, error_msg);
  }
  if (!eval.findVariable(name))
  {
    G4String error_msg = "Constant '" + name + "' is not defined!";
    G4Exception("DBEvaluator::GetConstant()", "InvalidSetup",
                FatalException, error_msg);
  }
  return Evaluate(name);
}

G4double DBEvaluator::GetVariable(const G4String& name)
{
  if (!IsVariable(name))
  {
    G4String error_msg = "Variable '" + name + "' is not a defined!";
    G4Exception("DBEvaluator::GetVariable()", "InvalidSetup",
                FatalException, error_msg);
  }
  return Evaluate(name);
}

G4String DBEvaluator::ConvertToString(G4int ival)
{
  std::ostringstream os;
  os << ival;
  G4String vl = os.str();
  return vl;
}

G4String DBEvaluator::ConvertToString(G4double dval)
{
  std::ostringstream os;
  os << dval;
  G4String vl = os.str();
  return vl;
}

} // - namespace COSMIC

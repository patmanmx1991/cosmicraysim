//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: DetectorMessenger.cc,v 1.8 2002/12/16 16:37:27 maire Exp $
// GEANT4 tag $Name: geant4-07-00-patch-01 $
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "db/ReadFile.hh"

namespace COSMIC {
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* dc_in) : fDC(dc_in)
{
  std::cout << "Setting up detector messenger" << std::endl;
  GeoDir = new G4UIdirectory("/geo/");
  GeoDir->SetGuidance("Geometry manager interface");

  InputValue = new G4UIcmdWithAString("/Cosmic/setgeo", this);
  InputValue->SetGuidance("Sets value in GEO table");
  InputValue->SetParameterName("GEO Table Inputs", false);
  InputValue->AvailableForStates(G4State_PreInit, G4State_Idle);

  UpdateGEO = new G4UIcmdWithoutParameter("/Cosmic/updategeo", this);
  UpdateGEO->SetGuidance("Updates the Geometry");
  UpdateGEO->AvailableForStates(G4State_PreInit, G4State_Idle);


  MessInput = new std::string;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{

  // if (command == InputValue) {
  //   UpdateGeoTable(newValue);
  // }

  // if (command == UpdateGEO) {
  //   fDC->Construct();
  // }

}

void DetectorMessenger::UpdateGeoTable(G4String newValue) {

  std::vector<std::string> parsedvals = COSMIC::DBUtils::ParseToStr(newValue);
  std::string index = parsedvals[0];
  std::string remai = "{ " + newValue.erase(newValue.find(index), index.size()) + " }";

  json::Reader reader(remai);
  json::Value jsonDoc;
  reader.getValue(jsonDoc);
  DBTable newTable = DBJSONParser::convertTablevals(jsonDoc);
  newTable.SetTableName("GEO");
  newTable.SetIndexName(index);

  if (DB::Get()->HasTable("GEO", index)) {
    DBTable clonetbl = DB::Get()->GetTable("GEO", index);
    newTable.UpdateFields(&clonetbl);
  }

  DB::Get()->AddTable(newTable);
}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


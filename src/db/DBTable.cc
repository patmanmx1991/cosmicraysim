#include "DBTable.hh"
#include "db/ReadFile.hh"
#include "db/DBEvaluator.hh"

namespace COSMIC {

// ---------------------------------------------------------------------------
DBTable::DBTable() {
}

DBTable::DBTable(std::string name, std::string ind) {
  table = json::Value();
  Set("name", name);
  Set("index", ind);
}

DBTable::DBTable(json::Value v) {
  table = v;
}

DBTable::DBTable(const DBTable &right) {
  table = right.table;
}

const DBTable& DBTable::operator=(const DBTable &right) {
  table = right.table;
  return *this;
}

DBTable DBTable::Clone() {
  DBTable left;
  std::vector<std::string> fields = table.getMembers();
  for (uint i = 0; i < fields.size(); i++) {
    json::Type type = GetType(fields[i]);
    std::string field = fields[i];

    if (type == json::TINTEGER) {
      left.table.setMember(field, json::Value(table[field].getInteger()));
    } else if (type == json::TUINTEGER) {
      left.table.setMember(field, json::Value(table[field].getUInteger()));
    } else if (type == json::TREAL) {
      left.table.setMember(field, json::Value(table[field].getReal()));
    } else if (type == json::TBOOL) {
      left.table.setMember(field, json::Value(table[field].getBool()));
    } else if (type == json::TSTRING) {
      left.table.setMember(field, json::Value(table[field].getString()));
    } else if (type == json::TBOOL_ARRAY) {
      std::vector<bool> vec = table[field].toVector<bool>();
      left.table.setMember(field, vec);
    } else if (type == json::TINTEGER_ARRAY) {
      std::vector<int> vec = table[field].toVector<int>();
      left.table.setMember(field, vec);
      } else if (type == json::TUINTEGER_ARRAY) {
      std::vector<uint> vec = table[field].toVector<uint>();
      left.table.setMember(field, vec);
    } else if (type == json::TREAL_ARRAY) {
      std::vector<double> vec = table[field].toVector<double>();
      left.table.setMember(field, vec);
    } else if (type == json::TSTRING_ARRAY) {
      std::vector<std::string> vec = table[field].toVector<std::string>();
      left.table.setMember(field, vec);
    } else if (type == json::TJSON) {

      DBTable jsontbl = DBTable(table.getMember(field));
      DBTable cltbl = jsontbl.Clone();
      left.table.setMember(field, cltbl.GetTable());

    }
  }
  return left;
}

DBTable::~DBTable() {
}

void DBTable::Set(std::string field, int i) {
  table.setMember(field, json::Value(i));
}

void DBTable::Set(std::string field, double d) {
  table.setMember(field, json::Value(d));
}

void DBTable::Set(std::string field, std::string s) {
  table.setMember(field, json::Value(s));
}

void DBTable::Set(std::string field, std::vector<int> iv) {
  table.setMember(field, json::Value(iv));
}

void DBTable::Set(std::string field, std::vector<double> dv) {
  table.setMember(field, json::Value(dv));
}

void DBTable::Set(std::string field, std::vector<std::string> sv) {
  table.setMember(field, json::Value(sv));
}

void DBTable::Set(std::string field, G4ThreeVector v){
  std::vector<double> vec;
  
  vec.push_back(v[1]);
  vec.push_back(v[2]);
  vec.push_back(v[3]);

  Set(field, vec);
}


void DBTable::SetIndexName(std::string s) {
  Set("index", s);
}

void DBTable::SetTableName(std::string s) {
  Set("name", s);
}

void DBTable::Prefix(std::string name, std::string pref) {
  std::string val = GetS(name);
  Set(name, pref + val);
  return;
}

void DBTable::Print() {
  std::cout << "----------------------" << std::endl;
  std::vector<std::string> fields = GetFields();
  for (std::vector<std::string>::iterator iter = fields.begin();
       iter != fields.end(); iter++) {
    std::string value = ""; //static_cast<std::string>(table.getMember((*iter)));
    std::cout << (*iter) << " ---> " << value << std::endl;
  }
  std::cout << "----------------------" << std::endl;
}

bool DBTable::Has(std::string field) {
  return table.isMember(field);
}

std::vector<std::string> DBTable::GetFields() {
  return table.getMembers();
}

void DBTable::ThrowNotFoundError(std::string s) {
  throw DBNotFoundError(GetTableName(), GetIndexName(), s);
}

json::Type DBTable::GetType(std::string field) {
  json::Value val = table[field];

  switch (val.getType()) {
  case json::TINTEGER:
  case json::TUINTEGER:
  case json::TBOOL:
  case json::TREAL:
  case json::TSTRING:
    return val.getType();
  case json::TARRAY:
    if (val.getArraySize() > 0) {
      switch (val[0].getType()) {
      case json::TINTEGER:
        return json::TINTEGER_ARRAY;
      case json::TUINTEGER:
        return json::TUINTEGER_ARRAY;
      case json::TBOOL:
        return json::TBOOL_ARRAY;
      case json::TREAL:
        return json::TREAL_ARRAY;
      case json::TSTRING:
        return json::TSTRING_ARRAY;
      default:
        return json::TJSON;
      }
    }
    return json::TJSON;
  default:
    return json::TJSON;
  }

}

bool DBTable::GetB(std::string field) {
  if (!Has(field)) ThrowNotFoundError(field);

  json::Type type = GetType(field);

  // Do standard Processing
  if (GetType(field) == json::TINTEGER) {
    return (table[field].getInteger()) > 0;

  } else if (GetType(field) == json::TUINTEGER) {
    return (table[field].getUInteger()) > 0;

  } else if (GetType(field) == json::TREAL) {

    return (table[field].getReal()) > 0;

  } else if (GetType(field) == json::TBOOL) {

    return bool(table[field].getBool());

  } else if (type == json::TSTRING) {

    // If its a string evaluate it using ROOT evaluator
    std::string value = table[field].getString();
    return Evaluate(value) > 0;

  } else {

    std::cout << "Incorrect TYPE" << std::endl;
    throw;
    return -1;

  }
}

int DBTable::GetI(std::string field) {
  if (!Has(field)) ThrowNotFoundError(field);

  json::Type type = GetType(field);

  // Do standard Processing
  if (GetType(field) == json::TINTEGER) {
    return int(table[field].getInteger());

  } else if (GetType(field) == json::TUINTEGER) {
    return int(table[field].getUInteger());

  } else if (GetType(field) == json::TREAL) {

    return int(table[field].getReal());

  } else if (GetType(field) == json::TBOOL) {

    return int(table[field].getBool());

  } else if (type == json::TSTRING) {

    // If its a string evaluate it using ROOT evaluator
    std::string value = table[field].getString();
    return Evaluate(value);

  } else {

    std::cout << "Incorrect TYPE" << std::endl;
    throw;
    return -1;

  }
}

float DBTable::GetF(std::string field) {
  return float(GetD(field));
}

double DBTable::GetD(std::string field) {
  if (!Has(field)) ThrowNotFoundError(field);

  // Do standard Processing
  if (GetType(field) == json::TINTEGER) {
    return double(table[field].getInteger());

  } else if (GetType(field) == json::TUINTEGER) {
    return double(table[field].getUInteger());

  } else if (GetType(field) == json::TREAL) {

    return double(table[field].getReal());

  } else if (GetType(field) == json::TBOOL) {

    return double(table[field].getBool());

  } else if (GetType(field) == json::TSTRING) {

    // If its a string evaluate it using ROOT evaluator
    std::string value = table[field].getString();
    return Evaluate(value);

  } else {

    std::cout << "Incorrect TYPE" << std::endl;
    throw;
    return -1;

  }
}

G4double DBTable::Evaluate(std::string value) {
  DBEvaluator evals;
  return evals.Evaluate(value);
}

std::string DBTable::GetS(std::string field) {
  if (not table.isMember(field)) ThrowNotFoundError(field);
  return table[field].getString();
}

DBTable* DBTable::GetT(std::string field) {
  if (not table.isMember(field)) ThrowNotFoundError(field);
  return new DBTable(table[field]);
}

G4double DBTable::GetUnits(std::string field) {
  
  if (Has(field + "_units")){
    G4String units = GetS(field + "_units");
    DBEvaluator evals;
    G4double unitsdouble = evals.Evaluate(units);
    return unitsdouble;

  } else if (Has("units")){
    G4String units = GetS("units");
    DBEvaluator evals;
    G4double unitsdouble = evals.Evaluate(units);
    return unitsdouble;
  }
  
  return 1.0;
}

G4double DBTable::GetG4D(std::string field) {
  return GetD(field) * GetUnits(field);
}

std::vector<G4double> DBTable::GetVecG4D(std::string field) {

  std::vector<G4double> dvec;

  if (GetType(field) == json::TREAL_ARRAY) {
    dvec = GetVecD(field);
    G4double tunits = GetUnits(field);
    for (uint i = 0; i < dvec.size(); i++) {
      dvec[i] *= tunits;
    }
    return dvec;

  }

  if (GetType(field) == json::TSTRING_ARRAY) {
    std::vector<std::string> svec = GetVecS(field);
    G4double tunits = GetUnits(field);
    for (uint i = 0; i < svec.size(); i++) {
      dvec.push_back( Evaluate(svec[i]) * tunits );
    }
    return dvec;
  }

  return dvec;
}

std::vector<int> DBTable::GetVecI(std::string field) {
  if (not table.isMember(field)) ThrowNotFoundError(field);
  return (table[field].toVector<int>());
}

std::vector<float> DBTable::GetVecF(std::string field) {
  if (not table.isMember(field)) ThrowNotFoundError(field);
  return (table[field].toVector<float>());
}

std::vector<double> DBTable::GetVecD(std::string field) {
  if (not table.isMember(field)) ThrowNotFoundError(field);
  return (table[field].toVector<double>());
}

std::vector<std::string> DBTable::GetVecS(std::string field) {
  if (not table.isMember(field)) ThrowNotFoundError(field);
  return table[field].toVector<std::string>();
}

std::string DBTable::GetTableName() {
  return GetS("name");
}

std::string DBTable::GetIndexName() {
  return GetS("index");
}

void DBTable::UpdateFields(DBTable* tbl) {
  std::vector<std::string> updates = tbl->GetFields();
  for (uint j = 0; j < updates.size(); j++) {
    std::string field = updates[j];
    if (this->Has(field)) continue;
    table.setMember(field, tbl->GetJSON().getMember(field));
  }
}

G4double DBTable::GetLengthUnits(std::string field){
  // If units found use them
  G4double units = GetUnits(field);
  if (units != 1.0){
    return units;
  }
  // Else look for "distance_units"
  units = GetUnits("length");
  if (units != 1.0){
    return units;
  }
  // Else default is m
  return m;
}

G4double DBTable::GetAngleUnits(std::string field){
  // If units found use them
  G4double units = GetUnits(field);
  if (units != 1.0){
    return units;
  }
  // Else look for "distance_units"
  units = GetUnits("angle");
  if (units != 1.0){
    return units;
  }
  // Else default is m
  return deg;
}

G4double DBTable::GetMassUnits(std::string field){
  // If units found use them
  G4double units = GetUnits(field);
  if (units != 1.0){
    return units;
  }
  // Else look for "distance_units"
  units = GetUnits("mass");
  if (units != 1.0){
    return units;
  }
  // Else default is m
  return kg;
}

G4double DBTable::GetEnergyUnits(std::string field){
  // If units found use them
  G4double units = GetUnits(field);
  if (units != 1.0){
    return units;
  }
  // Else look for "distance_units"
  units = GetUnits("energy");
  if (units != 1.0){
    return units;
  }
  // Else default is m
  return GeV;
}

G4ThreeVector DBTable::Get3DLength(std::string field){
  G4double units = GetLengthUnits(field);
  
  std::vector<double> pos = GetVecD(field);
  G4ThreeVector v(pos[0]*units,pos[1]*units,pos[2]*units);
  
  return v;
}

G4ThreeVector DBTable::Get3DPosition(std::string field){
  return Get3DLength(field);
}

G4ThreeVector DBTable::Get3DRotation(std::string field){
  G4double units = GetAngleUnits(field);
  
  std::vector<double> rot = GetVecD(field);
  G4ThreeVector v(rot[0]*units,rot[1]*units,rot[2]*units);
  
  return v;
}

G4double DBTable::GetLength(std::string field){
  return GetD(field) * GetLengthUnits(field);
}

G4double DBTable::GetAngle(std::string field){
  return GetD(field) * GetAngleUnits(field);
}

G4double DBTable::GetMass(std::string field){
  return GetD(field) * GetMassUnits(field);
}

G4double DBTable::GetEnergy(std::string field){
  return GetD(field) * GetEnergyUnits(field);
}
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
std::vector<DBTable*> DBJSONParser::parse(const std::string &filename)
{
  std::string contents;

  if (DBUtils::ReadFile(filename, contents) < 0) {
    std::cout << "FILE NOT FOUND " << std::endl;
    throw;
  }

  return parseString(contents);
}

std::vector<DBTable> DBJSONParser::parsevals(const std::string &filename)
{
  std::string contents;

  if (DBUtils::ReadFile(filename, contents) < 0) {
    std::cout << "FILE NOT FOUND " << std::endl;
    throw;
  }

  return parseStringvals(contents);
}

std::vector<DBTable> DBJSONParser::parseStringvals(const std::string &data)
{
  std::vector<DBTable> tables;

  json::Reader reader(data);
  json::Value jsonDoc;

  while (reader.getValue(jsonDoc) != 0) {
    DBTable newTable = convertTablevals(jsonDoc);
    tables.push_back(newTable);
  }

  return tables;
}
std::vector<DBTable *> DBJSONParser::parseString(const std::string &data)
{
  std::vector<DBTable *> tables;

  json::Reader reader(data);
  json::Value jsonDoc;

  while (reader.getValue(jsonDoc) != 0) {
    DBTable *newTable = convertTable(jsonDoc);
    tables.push_back(newTable);
  }

  return tables;
}

DBTable *DBJSONParser::convertTable(json::Value &jsonDoc)
{
  return new DBTable(jsonDoc);
}

DBTable DBJSONParser::convertTablevals(json::Value &jsonDoc)
{
  return DBTable(jsonDoc);
}
// ---------------------------------------------------------------------------
} // - namespace COSMIC

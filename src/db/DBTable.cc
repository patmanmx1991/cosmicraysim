#include "DBTable.hh"
#include "db/ReadFile.hh"
namespace COSMIC {

DBTable::DBTable()
{
}

DBTable::~DBTable()
{
  tblname.clear();
  index.clear();
}

DBTable::DBTable(json::Value v)
{
  table = v;
  tblname = GetS("name");
  index   = GetS("index");
}

std::vector<std::string> DBTable::GetFields() {
  return table.getMembers();
}

void DBTable::Print() {
  std::cout << "----------------------" << std::endl;
  std::vector<std::string> fields = GetFields();
  for (std::vector<std::string>::iterator iter = fields.begin();
       iter != fields.end(); iter++) {
    std::cout << (*iter) << " ---> " << "NOT YET IMPLEMENTED" << std::endl;
  }
  std::cout << "----------------------" << std::endl;
}

int DBTable::GetI(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return int(table[field].getReal());
}

float DBTable::GetF(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return float(table[field].getReal());
}


double DBTable::GetD(std::string field) {
  std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return double(table[field].getReal());
}

std::string DBTable::GetS(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return table[field].getString();
}

DBTable* DBTable::GetT(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return new DBTable(table[field]);
}

std::vector<int> DBTable::GetVecI(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return (table[field].toVector<int>());
}

std::vector<float> DBTable::GetVecF(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return (table[field].toVector<float>());
}

std::vector<double> DBTable::GetVecD(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return (table[field].toVector<double>());
}

std::vector<std::string> DBTable::GetVecS(std::string field) {
  // std::cout << "Getting field : " << field << std::endl;
  if (not table.isMember(field)) throw DBNotFoundError(tblname, index, field);
  return table[field].toVector<std::string>();
}


bool DBTable::Has(std::string field) {
  return table.isMember(field);
}


void DBTable::UpdateFields(DBTable* tbl) {
  std::vector<std::string> updates = tbl->GetFields();
  for (uint j = 0; j < updates.size(); j++) {
    std::string field = updates[j];
    if (this->Has(field)) continue;
    table.setMember(field, tbl->GetJSON().getMember(field));
  }
}

std::vector<DBTable*> DBJSONParser::parse(const std::string &filename)
{
  std::string contents;

  if (DBUtils::ReadFile(filename, contents) < 0) {
    std::cout << "FILE NOT FOUND " << std::endl;
    throw;
  }

  return parseString(contents);
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


}
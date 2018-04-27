#include "DB.hh"
#include <iostream>
#include <sstream>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "DBTable.hh"

namespace COSMIC {

DBNEW::DBNEW()
{
  CreateDataBase("default");
  SelectDataBase("default");
  fDefaultTables = &(fAllTables["default"]);
}

DBNEW::~DBNEW()
{
}

void DBNEW::CreateDataBase(std::string dataid)
{
  std::cout << "DB : --> Creating Database : " << dataid << std::endl;
  fAllTables[dataid] = std::vector<DBTable>();
}

void DBNEW::SelectDataBase(std::string dataid)
{
  std::cout << "DB : --> Selecting Database : " << dataid << std::endl;
  fCurrentTables = &(fAllTables[dataid]);
}

int DBNEW::Load(std::string filename)
{
  // Try to get file info assuming the name is literal
  struct stat s;
  stat(filename.c_str(), &s);
  std::string f = filename;
  std::string suf = ".db";

  if (S_ISDIR(s.st_mode)) {
    return LoadAll(filename);
  } else if (f.rfind(suf) == (f.size() - suf.size())) {
    return LoadFile(filename);
  } else {
    return 1;
  }

  std::cout << "Cannot Load Data Tables : " << filename << std::endl;
  throw;
  return 0;
}

int DBNEW::LoadFile(std::string filename)
{
  std::cout << "DB : --> Loading '" << filename << "'' ... ";
  // Try to read and parse it as databse or JSON file.
  std::vector<DBTable> contents = DBJSONParser::parsevals(filename);

  std::vector<DBTable>::iterator itbl;
  for (itbl = contents.begin(); itbl != contents.end(); itbl++) {
    DBTable table = (*itbl);
    (*fCurrentTables).push_back(table);
  }

  std::cout << " --> Complete. (Loaded " << contents.size() << " Tables)" << std::endl;
  Finalise();
  return 1;
}

int DBNEW::LoadAll(std::string dirname, std::string pattern)
{
  std::cout << "DB : Loading all DB files from " << dirname << pattern << std::endl;
  pattern = dirname + "/*"; //pattern;
  glob_t g;

  if (glob(pattern.c_str(), 0, 0, &g) == 0) {
    for (unsigned i = 0; i < g.gl_pathc; i++) {
      std::string path(g.gl_pathv[i]);
      //std::cout << "DBNEW: Loading " << path << " ... ";
      if (!Load(path)) {
        std::cout << "Load Failed!" << std::endl;
        globfree(&g);
        return 0;
      }
    }
  }
  globfree(&g);
  return 1;
}

void DBNEW::Finalise() {

  // To finalise the database we want to expand all databases
  // that have the entry "clone_table" in them.
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    DBTable tbl = (*fCurrentTables)[i];
    if (!tbl.Has("clone")) continue;

    // Get the table to be cloned
    std::string clonename = tbl.GetS("clone");
    DBTable clonetbl = GetTable(tbl.GetTableName(), clonename);

    // Make new table from clone
    DBTable newtbl = DBTable(tbl.GetJSON());

    // Loop over fields and update
    newtbl.UpdateFields(&clonetbl);
    // newtbl->SetIndexName(tbl->GetIndexName());
    (*fCurrentTables)[i] = newtbl;

  }
}

DBTable  DBNEW::GetTable (std::string tablename, std::string index) {
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    if (index.compare((*fCurrentTables)[i].GetIndexName()) != 0) continue;
    return (*fCurrentTables)[i].Clone();
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      if (index.compare((*fDefaultTables)[i].GetIndexName()) != 0) continue;
      return (*fDefaultTables)[i].Clone();
    }
  }
  throw DBNotFoundError(tablename, index, "");
  return fNullTable;
}

DBTable* DBNEW::GetLink  (std::string tablename, std::string index) {
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    if (index.compare((*fCurrentTables)[i].GetIndexName()) != 0) continue;
    return &(*fCurrentTables)[i];
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      if (index.compare((*fDefaultTables)[i].GetIndexName()) != 0) continue;
      return &(*fDefaultTables)[i];
    }
  }
  throw DBNotFoundError(tablename, index, "");
  return 0;
}

std::vector<DBTable>  DBNEW::GetTableGroup (std::string tablename) {
  std::vector<DBTable> tableset;
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    std::string index = (*fCurrentTables)[i].GetIndexName();
    tableset.push_back((*fCurrentTables)[i].Clone());
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      std::string index = (*fDefaultTables)[i].GetIndexName();
      tableset.push_back((*fDefaultTables)[i].Clone());
    }
  }
  return tableset;
}

std::vector<DBTable*> DBNEW::GetLinkGroup  (std::string tablename) {
  std::vector<DBTable*> tableset;
  for (uint i = 0; i < (*fCurrentTables).size(); i++) {
    if (tablename.compare((*fCurrentTables)[i].GetTableName()) != 0) continue;
    std::string index = (*fCurrentTables)[i].GetIndexName();
    tableset.push_back(&(*fCurrentTables)[i]);
  }
  if (fCurrentTables != fDefaultTables) {
    for (uint i = 0; i < (*fDefaultTables).size(); i++) {
      if (tablename.compare((*fDefaultTables)[i].GetTableName()) != 0) continue;
      std::string index = (*fDefaultTables)[i].GetIndexName();
      tableset.push_back(&(*fDefaultTables)[i]);
    }
  }
  return tableset;
}

void DBNEW::AddTable(DBTable tbl) {
  fCurrentTables->push_back(tbl);
}

DBNEW *DBNEW::fPrimary(0);

std::string DBNEW::GetDataPath() {
  std::string datadir = std::string(getenv("GLG4DATA"));
  return datadir;
}

void DBNEW::PrintSplashScreen() {
  std::cout << "=========================================" << std::endl;
  std::cout << "\n COSMIC RAY SIM : Cosmic Rays for Environmental Science and Technology Applications" << std::endl;
  std::cout << "\n\n Authors: Patrick Stowell, Lee Thompson" << std::endl;
  std::cout << " contact : p.stowell@sheffield.ac.uk" << std::endl;
  std::cout << "\n=========================================" << std::endl;
}

} // namespace COSMIC

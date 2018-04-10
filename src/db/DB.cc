#include "DB.hh"
#include <iostream>
#include <sstream>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "DBTable.hh"

namespace COSMIC {

DB *DB::fPrimary(0);

std::string DB::GetDataPath(){
  std::string datadir = std::string(getenv("GLG4DATA"));
  return datadir;
}


void DB::PrintSplashScreen(){
  std::cout << "=========================================" << std::endl;
  std::cout << "\n CRESTA : Cosmic Rays for Environmental Science and Technology Applications" << std::endl;
  std::cout << "\n\n Authors: Chris Steer, Patrick Stowell, Lee Thompson" << std::endl;
  std::cout << " contact : p.stowell@sheffield.ac.uk" << std::endl;
  std::cout << "\n =========================================" << std::endl;
}

DB::DB()
{
}

DB::~DB()
{
}

int DB::Load(std::string filename)
{
    std::cout << "Trying to Load : " << filename << std::endl;
  // Try to get file info assuming the name is literal
  struct stat s;
  stat(filename.c_str(), &s);
  std::string f = filename;
  std::string suf = ".db";


  // std::cout << "DB: Loading " << filename << "\n";
  if (S_ISDIR(s.st_mode)){
    return LoadAll(filename);
  } else if (f.rfind(suf) == (f.size() - suf.size())){
    std::cout << "Trying to LoadFile" << std::endl;
    return LoadFile(filename);
  } else {
    return 1;
  } 
  
  std::cout << "Cannot Load Stuff!" << std::endl;
  throw;
  return 0; // Never get here
}

int DB::LoadFile(std::string filename)
{
  std::cout << "DB: Loading '" << filename << "'' ... ";
  // Try to read and parse it as databse or JSON file.
  std::vector<DBTable *> contents;
  contents = ReadDBFile(filename);
  
  std::vector<DBTable *>::iterator itbl;
  for (itbl=contents.begin(); itbl != contents.end(); itbl++) {
    DBTable *table = (*itbl);  
    fAllTables.push_back(table);
  }

  std::cout << " --> Complete. (Loaded " << contents.size() << " Tables)" << std::endl;
  return 1;
}

int DB::LoadAll(std::string dirname, std::string pattern)
{
  std::cout << "DB: Loading all DB files from " << dirname << pattern << std::endl;
  pattern = dirname + "/*"; //pattern;
  glob_t g;

  if (glob(pattern.c_str(), 0, 0, &g) == 0) {
    for (unsigned i=0; i < g.gl_pathc; i++) {
      std::string path(g.gl_pathv[i]);
      //std::cout << "DB: Loading " << path << " ... ";
      if (!Load(path)){
        std::cout << "Load Failed!" << std::endl;
        globfree(&g);
        return 0;
      } 
    }
  }
  globfree(&g);
  return 1;
}

void DB::Finalise(){

  // To finalise the database we want to expand all databases
  // that have the entry "clone_table" in them.
  for (uint i = 0; i < fAllTables.size(); i++){
    DBTable* tbl = fAllTables[i];
    if (!tbl->Has("clone")) continue;

    // Get the table to be cloned
    std::string clonename = tbl->GetS("clone");
    DBTable* clonetbl = GetTable(tbl->GetTableName(), clonename);

    // Make new table from clone
    DBTable* newtbl = new DBTable(tbl->GetJSON());

    // Loop over fields and update
    newtbl->UpdateFields(clonetbl);
    // newtbl->SetIndexName(tbl->GetIndexName());
    fAllTables[i] = newtbl;

  }

}

std::vector<DBTable *> DB::ReadDBFile(const std::string &filename)
{
  std::vector<DBTable *> contents;
  contents = DBJSONParser::parse(filename);
  return contents;
}

DBTable* DB::GetTable(std::string tablename, std::string index){
  for (uint i = 0; i < fAllTables.size(); i++){
    if (tablename.compare(fAllTables[i]->GetTableName()) != 0) continue;
    if (index.compare(fAllTables[i]->GetIndexName()) != 0) continue;
    return fAllTables[i];
  }
  throw DBNotFoundError(tablename, index, "");
  return 0;
}

DBLink* DB::GetLink(std::string tablename, std::string index){
  for (uint i = 0; i < fAllTables.size(); i++){
    if (tablename.compare(fAllTables[i]->GetTableName()) != 0) continue;
    if (index.compare(fAllTables[i]->GetIndexName()) != 0) continue;
    return new DBLink(this, fAllTables[i], tablename, index);
  }
  throw DBNotFoundError(tablename, index, "");  
  return 0; //DBLink();
}


std::vector<DBLink*> DB::GetLinkGroup(std::string tablename){
  std::vector<DBLink*> tableset;
  for (uint i = 0; i < fAllTables.size(); i++){
    if (tablename.compare(fAllTables[i]->GetTableName()) != 0) continue;
    std::string index = fAllTables[i]->GetIndexName();
    tableset.push_back(new DBLink(this, fAllTables[i], tablename, index));
  }
  if (tableset.size() == 0) throw DBNotFoundError(tablename, "", "");  
  return tableset;
}






} // namespace COSMIC
#ifndef __COSMIC_DB_HH__
#define __COSMIC_DB_HH__

#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>

#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>

#include "db/DBTable.hh"

namespace COSMIC {

class DB {
public:

  // Make a static way to access the DB
  static inline DB *Get()
  { return fPrimary == 0 ? fPrimary = new DB : fPrimary; };
  static DB *fPrimary;

  DB();
  ~DB();
  DB(std::string filename);

  static std::string GetDataPath();
  static void PrintSplashScreen();

  void CreateDataBase(std::string dataid);

  void SelectDataBase(std::string dataid);

  bool HasTables(std::string name);
  bool HasTable(std::string name, std::string index);

  int Load(std::string filename);
  int LoadAll(std::string dirname, std::string pattern = "/*");
  int LoadFile(std::string filename);
  void Finalise();

  DBTable  GetTable (std::string tablename, std::string index);
  DBTable  CreateTable (std::string tablename, std::string index);

  DBTable* GetLink  (std::string tablename, std::string index);

  std::vector<DBTable>  GetTableGroup (std::string tablename);
  std::vector<DBTable*> GetLinkGroup  (std::string tablename);

  void AddTable(DBTable tbl);

  inline void SetOutputFile(std::string filename) { fOutputFile = filename; };
  inline std::string GetOutputFile() { return fOutputFile; };

  static std::vector<double> RedColor();
  static std::vector<double> BlueColor();


protected:

  std::map<std::string, std::vector<DBTable> > fAllTables;
  std::vector<DBTable>* fCurrentTables;
  std::vector<DBTable>* fDefaultTables;
  std::string fOutputFile;
  DBTable fNullTable;
  int fDataBaseCalls;

};
} // - namespace COSMIC
#endif

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
#include "db/DBLink.hh"


#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>


namespace COSMIC{

class DB;
class DBTable;

class DB {
public:

  DB();
  virtual ~DB();

  // Make a static way to access the DB
  static inline DB *Get()
    { return fPrimary == 0 ? fPrimary = new DB : fPrimary; };
  static std::string GetDataPath();
  static void PrintSplashScreen();
  
  static DB *fPrimary;

  int Load(std::string filename);
  int LoadAll(std::string dirname, std::string pattern="/*");
  int LoadFile(std::string filename);
  inline void SetOutputFile(std::string filename) { fOutputFile = filename; };
  inline std::string GetOutputFile(){ return fOutputFile; };
  void Finalise();

  std::vector<DBTable*> ReadDBFile(const std::string &filename);

  DBTable* GetTable(std::string tablename, std::string index);

  DBLink* GetLink(std::string tablename, std::string index);
  std::vector<DBLink*> GetLinkGroup(std::string tablename);
  

protected:
  std::vector<DBTable*> fAllTables;
  std::string fOutputFile;
};

















} // namespace COSMIC
#endif
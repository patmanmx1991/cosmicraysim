#ifndef __DBLINK_HH__
#define __DBLINK_HH__
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
#include "DBTable.hh"

namespace COSMIC{

class DB;
class DBLink;



class DBLink {
public:
  DBLink();
  DBLink(DB *db, DBTable* dbt, std::string tblname, std::string index);
  ~DBLink();

  std::string GetTableName();
  std::string GetIndexName();
  std::vector<std::string> GetFields();

  bool Has(std::string name);
  
  bool GetB(std::string name);
  int GetI(std::string name);
  float GetF(std::string name);
  double GetD(std::string name);
  std::string GetS(std::string name);

  std::vector<int> GetVecI(std::string name);
  std::vector<float> GetVecF(std::string name);
  std::vector<double> GetVecD(std::string name);
  std::vector<std::string> GetVecS(std::string name);

  void Print();

  void Lock();
  void Unlock();

  /** Pointer to DB which created this link. */
  DB *db;
  /** Pointer to DBTable which is protected so people can't delete it. */
  DBTable* dbtable;
  std::string tblname;
  std::string index;
  /** Lock : The only way to access DB values is through a link and users have to manually unlock them
  If they want to edit them to avoid mistaken edits in the code. **/
  bool fLocked;
};

typedef std::map<std::string, DBLink> DBLinkGroup;
}
#endif
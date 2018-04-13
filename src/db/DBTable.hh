#ifndef __DBTABLE_HH__
#define __DBTABLE_HH__

#include <string>
#include <vector>
#include <iostream>
#include <db/json.hh>
#include <db/ReadFile.hh>


namespace COSMIC
{

// Forward Declarations
class DBTable;
class DBJSONParser;

// Database Table Class 
class DBTable{
public:
  // Constructors
  DBTable();
  DBTable(json::Value v);
  DBTable(std::string name, std::string ind);

  virtual ~DBTable();

  // Access Functions
  inline std::string GetTableName(){return tblname;};
  inline std::string GetIndexName(){return index;};

  void Print();
  std::vector<std::string> GetFields();

  bool Has(std::string name);

  void Set(std::string name, int i);
  void Set(std::string name, std::string s);
  void Set(std::string name, std::vector<double> dv);

  int GetI(std::string name);
  float GetF(std::string name);
  double GetD(std::string name);
  std::string GetS(std::string name);
  DBTable* GetT(std::string name);
  
  std::vector<int> GetVecI(std::string name);
  std::vector<float> GetVecF(std::string name);
  std::vector<double> GetVecD(std::string name);
  std::vector<std::string> GetVecS(std::string name);

  inline json::Value GetJSON(){return table;};

  void UpdateFields(DBTable* overrides);
  inline void SetIndexName(std::string ind){ index = ind; Set("index",ind); };
  inline void SetTableName(std::string ind){ tblname = ind; Set("name",ind); };
  void Prefix(std::string name, std::string pref);
  
protected:
  std::string tblname; 
  std::string index;   
  json::Value table;
};

// Parser class for static calls
class DBJSONParser
{
public:
  /** Returns a list of all tables found in JSON text file @p filename. */
  static std::vector<DBTable *> parse(const std::string &filename);

  /** Returns a list of all tables found in JSON string. */
  static std::vector<DBTable *> parseString(const std::string &data);
  
  /** Converts a JSON document to a RATDB table */
  static DBTable *convertTable(json::Value &jsonDoc);
};



/** Exception: Field not found in DB */
class DBNotFoundError {
public:
  /** Create new exception.
   *
   *  @param _table  Name of table in which field could not be found
   *  @param _index  Index of table in which field could not be found
   *  @param _field  Name of field which could not be found
   */
  DBNotFoundError (const std::string &_table, const std::string &_index, 
          const std::string &_field) :
    table(_table), index(_index), field(_field) {
      std::cout << "DBNotFoundError : " << table << " : " << index << " : " << field << std::endl;
  };

  /** Compare if @p other has the same table, index and field as @p this. */
  bool operator== (const DBNotFoundError &other) const {
    return table == other.table && index == other.index && field == other.field;
  }

  std::string table;
  std::string index;
  std::string field;
};




}

#endif
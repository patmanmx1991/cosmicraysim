#ifndef __DBTABLE_HH__
#define __DBTABLE_HH__

#include <string>
#include <vector>
#include <iostream>

#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "G4GDMLEvaluator.hh"

#include "db/ROOTHeaders.hh"
#include "db/DBEvaluator.hh"
#include "db/json.hh"
#include "db/ReadFile.hh"

namespace COSMIC {

// ---------------------------------------------------------------------------
/// Database Table Class. A wrapper around the json table reader
class DBTable {
public:

  /// Empty constructor
  DBTable();
  /// Constructor from other JSON
  DBTable(json::Value v);
  /// Constructor setting name and index
  DBTable(std::string name, std::string ind);
  /// Copy Constructor
  DBTable(const DBTable &right);
  /// Copy operator
  const DBTable& operator=(const DBTable &right);
  /// Destructor, wipes JSON info
  ~DBTable();
  /// Creates a cloned object
  DBTable Clone();

  // Setting functions, change or add stuff to JSON
  void Set(std::string field, int i);                       ///< Set integer value
  void Set(std::string field, double i);                    ///< Set integer value
  void Set(std::string field, std::string s);               ///< Set std::string value
  void Set(std::string field, std::vector<int> iv);         ///< Set vector int
  void Set(std::string field, std::vector<double> dv);      ///< Set vector double
  void Set(std::string field, std::vector<std::string> sv); ///< Set vector double
  void Set(std::string field, G4ThreeVector v); ///< Set from 3D vector

  void SetTableName(std::string n); ///< Special table ID set function
  void SetIndexName(std::string n); ///< Special index ID set function

  void Prefix(std::string name, std::string pref); ///< Prefix a string entry with another string value


  /// Print out details in the table
  void Print();

  std::string GetTableName(); ///< Special name request
  std::string GetIndexName(); ///< Special index request


  // Request functions
  bool Has(std::string field); ///< Return if field is in this table
  std::vector<std::string> GetFields(); ///< Return list of all fields present
  json::Type GetType(std::string field); ///< Return JSON Type Member

  bool GetB(std::string name);
  int GetI(std::string name);
  float GetF(std::string name);
  double GetD(std::string name);
  std::string GetS(std::string name);

  std::vector<bool>        GetVecB(std::string name);
  std::vector<int>         GetVecI(std::string name);
  std::vector<float>       GetVecF(std::string name);
  std::vector<double>      GetVecD(std::string name);
  std::vector<std::string> GetVecS(std::string name);

  G4double Evaluate(std::string value); ///< Take a string and substitude other variables
  G4double GetUnits(std::string field); ///< Get G4double units to go with value
  G4double GetG4D(std::string name);    ///< Get G4double object with units
  std::vector<G4double>    GetVecG4D(std::string name); ///< Get vector of G4double objects

  G4double GetLengthUnits(std::string field);
  G4double GetAngleUnits(std::string field);
  G4double GetMassUnits(std::string field);
  G4double GetEnergyUnits(std::string field);

  G4ThreeVector Get3DPosition(std::string field);
  G4ThreeVector Get3DLength(std::string field);
  G4ThreeVector Get3DRotation(std::string field);

  G4double      GetLength(std::string field);
  G4double      GetAngle(std::string field);
  G4double      GetMass(std::string field);
  G4double      GetEnergy(std::string field);

  /// Return the actual table objecy
  inline json::Value GetTable() { return table; };

  // Deprecated REQUESTS
  DBTable* GetT(std::string name);
  inline json::Value GetJSON() {return table;};
  void UpdateFields(DBTable* overrides);

protected:

  void ThrowNotFoundError(std::string s); ///< Easier function call to throw errors
  json::Value table; ///< Actual JSON value we are wrapping around
};


// ---------------------------------------------------------------------------
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

    /** Returns a list of all tables found in JSON text file @p filename. */
  static std::vector<DBTable> parsevals(const std::string &filename);

  /** Returns a list of all tables found in JSON string. */
  static std::vector<DBTable> parseStringvals(const std::string &data);

  /** Converts a JSON document to a RATDB table */
  static DBTable convertTablevals(json::Value &jsonDoc);
};
// ---------------------------------------------------------------------------



// ---------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------
} // - namespace COSMIC


// ---------------------------------------------------------------------------
class DBROOTIO {
public:
  DBROOTIO() {};
  ~DBROOTIO() {};
  std::vector<std::string> fIndex;
  std::vector<std::string> fFields;
  std::vector<std::string> fValues;
  std::vector<std::string> fTypes;
  // ClassDef (DBROOTIO, 1);
};
// ---------------------------------------------------------------------------
#endif

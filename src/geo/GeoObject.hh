#ifndef __GEO_OBJECT_HH__
#define __GEO_OBJECT_HH__

#include "db/DBTable.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;
class G4VSensitiveDetector;

namespace COSMIC {

/// Virtual GeoObject Template defining functions to
/// return each of the physical/logical volume pointers.
class GeoObject {
public:

  /// Set name and type in constructor
  inline GeoObject(std::string id, std::string type) :
    fID(id),
    fType(type),
    fLogical(NULL),
    fPhysical(NULL),
    fMotherLogical(NULL),
    fMotherPhysical(NULL) {};

  /// Simple empty constructor
  inline GeoObject() :
    fLogical(NULL),
    fPhysical(NULL),
    fMotherLogical(NULL),
    fMotherPhysical(NULL) {};

  /// Destructor
  inline ~GeoObject() {};

  /// Get this objects unique ID
  inline std::string GetID() { return fID; };
  /// Set this objects unique ID
  inline void SetID(std::string id){ fID = id; };

    /// Get this objects type
  inline std::string GetType() { return fType; };
  /// Set this objects type
  inline void SetType(std::string type){ fType = type; };

  /// Get this objects logical volume
  inline G4LogicalVolume* GetLogical() { return fLogical; };
  /// Set this objects logical volume
  inline void SetLogical(G4LogicalVolume* lg) { fLogical = lg; };

  /// Return the logical volume of this objects mother
  inline G4LogicalVolume* GetMotherLogical() { return fMotherLogical; };
  /// Set this objects logical volume
  inline void SetMotherLogical(G4LogicalVolume* mlg) { fMotherLogical = mlg; };

  /// Get this objects physical volume
  inline G4VPhysicalVolume* GetPhysical() { return fPhysical; };
  /// Set this objects physical volume
  inline void SetPhysicalVolume(G4VPhysicalVolume* pv){ fPhysical = pv; };

  /// Return the physical volume of this objects mother
  // inline G4VPhysicalVolume* GetMotherPhysical() { return fMotherPhysical; }
  /// Set this objects physical mother volume pointer
  // inline void SetMotherPhysical(G4VPhysicalVolume* mpv){ fMotherPhysical = mpv; };

  inline virtual G4VSensitiveDetector* GetSensitive(){ return fSensitive; };
  inline virtual void SetSensitive(G4VSensitiveDetector* s){ fSensitive = s; };

  /// Build this GeoObject from a table
  virtual void Construct(DBTable /*table*/) {};

  /// Construct the sensitive part ! REMOVE!
  // virtual void ConstructSensitive(DB* db) {};


protected:

  std::string fID;   ///< Unique ID for this geometry
  std::string fType; ///< Type string of this geometry

  G4LogicalVolume* fLogical;           ///< Pointer to logical volume
  G4VPhysicalVolume* fPhysical;        ///< Pointer to physical volume
  G4LogicalVolume* fMotherLogical;     ///< Pointer to mother logic
  G4VPhysicalVolume* fMotherPhysical;  ///< Pointer to mother physical
  G4VSensitiveDetector* fSensitive; ///< Pointer to sensitive detector object for this geometry

};

} // namespace COSMIC
#endif

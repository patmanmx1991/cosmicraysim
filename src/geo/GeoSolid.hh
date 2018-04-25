#ifndef __GEOSOLID_HH__
#define __GEOSOLID_HH__

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "GeoObject.hh"
#include "GeoUtils.hh"
#include "db/DBLink.hh"
#include "db/DB.hh"
#include "materials/MaterialManager.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

/// Solid geometry object, building on the GeoObject template.
/// Contains all functions to produce logical and physical volumes
/// from a DBLink.
class GeoSolid : public GeoObject {
public:

  /// Empty Constructor
  GeoSolid();

  /// Function to actually create each volume from a table.
  void Construct(DBLink* table);

  /// Create the solid volume for this objet. Children must implement.
  virtual G4VSolid *ConstructSolidVolume(DBLink* table) = 0;
  
  /// Construct logical volume given materials etc in a table
  virtual G4LogicalVolume *ConstructLogicalVolume(DBLink* table, G4VSolid* solid);

  /// Construct a physical volume placement given the settings in a table and the provided mother+logic
  virtual G4VPhysicalVolume* ConstructPhysicalVolume(DBLink* table, G4LogicalVolume* mother, G4LogicalVolume* logic);

  /// Construct a sensitive detector from the table and attach to the logic
  virtual G4VSensitiveDetector* ConstructSensitiveDetector(DBLink* table, G4LogicalVolume* logic, G4VPhysicalVolume* vol);

  /// Construct a physical volume placement given the settings in a table and the provided mother+logic
  virtual G4VPhysicalVolume* ConstructPhysicalReplica(DBLink* table, G4LogicalVolume* mother, G4LogicalVolume* logic);
 
  /// Construct a physical volume placement given the settings in a table and the provided mother+logic
  virtual G4VPhysicalVolume* ConstructPhysicalPlacement(DBLink* table, G4LogicalVolume* mother, G4LogicalVolume* logic);

  /// Construct a physical volume placement given the settings in a table and the provided mother+logic
  virtual G4VPhysicalVolume* ConstructPhysicalParametrisation(DBLink* table, G4LogicalVolume* mother, G4LogicalVolume* logic);


protected:

  G4VSolid* fSolid; ///< Geant4 solid object for this geometry

};

} // namespace COSMIC
#endif
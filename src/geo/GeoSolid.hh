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

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

class GeoSolid : public GeoObject {
public:
  inline GeoSolid(){};
  inline GeoSolid(DBLink* table) {Construct(table);};
  virtual G4VSolid *ConstructSolidVolume(DBLink* table) = 0;
  virtual G4LogicalVolume *ConstructLogicalVolume(DBLink* table, G4VSolid* solid);
  virtual G4VPhysicalVolume* ConstructPhysicalVolume(DBLink* table, G4LogicalVolume* mother, G4LogicalVolume* logic);
  virtual G4VSensitiveDetector* ConstructSensitiveDetector(DBLink* table, G4LogicalVolume* logic);
  virtual void ConstructSensitive();
  
  void Construct(DBLink* table);  
  G4VSolid* fSolid;
  G4VSensitiveDetector* fSensitive;
};

} // namespace COSMIC
#endif
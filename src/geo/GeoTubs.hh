#ifndef __GEO_TUBS__HH__
#define __GEO_TUBS__HH__

#include "GeoSolid.hh"

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

namespace COSMIC {

class GeoTubs : public GeoSolid {
public:
  inline GeoTubs(){};
  inline GeoTubs(DBLink* table) {Construct(table);};
  G4VSolid *ConstructSolidVolume(DBLink* table);
};

} // namespace COSMIC
#endif


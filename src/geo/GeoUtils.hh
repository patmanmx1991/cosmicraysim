#ifndef __GEOUTILS_HH__
#define __GEOUTILS_HH__

#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"

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

namespace COSMIC{
  namespace GEO{
    G4VSolid* GetSolidFromStore(std::string name, bool verbose=true);
    G4Box* GetBoxFromStore(std::string name, bool verbose=true);
    G4LogicalVolume* GetLogicalFromStore(std::string name, bool verbose=true);
    G4LogicalVolume* GetMotherLogicalFromStore(DBLink* tb, bool verbose=true);

  }
}

#endif
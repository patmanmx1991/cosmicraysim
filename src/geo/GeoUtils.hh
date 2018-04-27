#ifndef __GEOUTILS_HH__
#define __GEOUTILS_HH__

#include <iostream>
#include "G4Box.hh"
#include "G4SolidStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "db/DBTable.hh"

namespace COSMIC {

/// GEO Utils namespace
namespace GEO {

/// Get a solid from the Geant4 solid store via name
G4VSolid* GetSolidFromStore(std::string name, bool verbose = true);
/// Get a G4Box from the Geant4 solid store via name
G4Box* GetBoxFromStore(std::string name, bool verbose = true);
/// Get a logical volume from the Geant4 logical volume store via name
G4LogicalVolume* GetLogicalFromStore(std::string name, bool verbose = true);
/// Get a mother logical volume from the store via a table
G4LogicalVolume* GetMotherLogicalFromStore(DBTable tb, bool verbose = true);

} // - namespace GEO
} // - namespace COSMIC

#endif

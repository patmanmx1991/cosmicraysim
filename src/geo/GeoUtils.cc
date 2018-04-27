#include "GeoUtils.hh"

namespace COSMIC {
namespace GEO {

G4VSolid* GetSolidFromStore(std::string name, bool verbose) {
  return G4SolidStore::GetInstance()->GetSolid(name, verbose);
}

G4Box* GetBoxFromStore(std::string name, bool verbose) {
  G4VSolid* solid = G4SolidStore::GetInstance()->GetSolid(name, verbose);
  G4Box* box = static_cast<G4Box*>(solid);
  return box;
}

G4LogicalVolume* GetLogicalFromStore(std::string name, bool verbose) {
  return G4LogicalVolumeStore::GetInstance()->GetVolume(name, verbose);
}

G4LogicalVolume* GetMotherLogicalFromStore(DBTable tb, bool verbose) {
  if (!tb.Has("mother")) return 0;
  else return GetLogicalFromStore(tb.GetS("mother"), verbose);
}

} // - namespace GEO
} // - namespace COSMIC

#include "GeoTubs.hh"

namespace COSMIC {

G4VSolid* GeoTubs::ConstructSolidVolume(DBLink* tbl){
  std::string geo_id = tbl->GetIndexName();
  G4double r_max = tbl->GetD("r_max") * CLHEP::m;
  G4double size_z = tbl->GetD("size_z") * CLHEP::m;

  G4double r_min = tbl->Has("r_min") ? tbl->GetD("r_min") : 0.0;
  G4double phi_start = tbl->Has("phi_start") ? tbl->GetD("phi_start") : 0.0;
  G4double phi_delta = tbl->Has("phi_delta") ? tbl->GetD("phi_delta") : CLHEP::twopi;

  return new G4Tubs(geo_id, r_min, r_max, size_z,
                    phi_start, phi_delta);
}
}
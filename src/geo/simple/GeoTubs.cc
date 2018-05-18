#include "GeoTubs.hh"

#include "geo/GeoSolid.hh"
#include "G4Box.hh"

namespace COSMIC {

GeoTubs::GeoTubs(DBTable table)
{
  Construct(table);
};

G4VSolid* GeoTubs::ConstructSolidVolume(DBTable tbl) {
  std::string geo_id = tbl.GetIndexName();
  G4double r_max     = tbl.GetG4D("r_max");
  G4double size_z    = tbl.GetG4D("size_z")*0.5;

  G4double r_min = tbl.Has("r_min") ? tbl.GetG4D("r_min") : 0.0;
  G4double phi_start = tbl.Has("phi_start") ? tbl.GetG4D("phi_start") : 0.0;
  G4double phi_delta = tbl.Has("phi_delta") ? tbl.GetG4D("phi_delta") : CLHEP::twopi;

  return new G4Tubs(geo_id, r_min, r_max, size_z,
                    phi_start, phi_delta);
}

} // - namespace COSMIC

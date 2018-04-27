#include "GeoCutTubs.hh"

#include "GeoSolid.hh"
#include "G4Box.hh"

namespace COSMIC {

GeoCutTubs::GeoCutTubs(DBTable table)
{
  Construct(table);
}

G4VSolid* GeoCutTubs::ConstructSolidVolume(DBTable tbl) {
  std::string geo_id = tbl.GetIndexName();
  G4double r_max     = tbl.GetG4D("r_max");
  G4double size_z    = tbl.GetG4D("size_z")*0.5;

  G4double r_min = tbl.Has("r_min") ? tbl.GetG4D("r_min") : 0.0;
  G4double phi_start = tbl.Has("phi_start") ? tbl.GetG4D("phi_start") : 0.0;
  G4double phi_delta = tbl.Has("phi_delta") ? tbl.GetG4D("phi_delta") : CLHEP::twopi;

  G4ThreeVector plow;
  if (tbl.Has("p_low")){
    std::vector<G4double> plowtemp = tbl.GetVecG4D("p_low");
    plow[0] = plowtemp[0];
    plow[1] = plowtemp[1];
    plow[2] = plowtemp[2];
  }

  G4ThreeVector phigh;
  if (tbl.Has("p_high")){
    std::vector<G4double> phightemp = tbl.GetVecG4D("p_high");
    phigh[0] = phightemp[0];
    phigh[1] = phightemp[1];
    phigh[2] = phightemp[2];
  }

  return new G4CutTubs(geo_id, 
		       r_min, 
		       r_max, 
		       size_z,
		       phi_start, 
		       phi_delta,
		       plow,
		       phigh);
}

} // - namespace COSMIC

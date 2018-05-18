#include "GeoCone.hh"
#include "G4Cons.hh"
#include "geo/GeoSolid.hh"


namespace COSMIC {

GeoCone::GeoCone(DBTable table)
{
  Construct(table);
};

G4VSolid* GeoCone::ConstructSolidVolume(DBTable tbl) {
  std::string geo_id = tbl.GetIndexName();
  G4double r_max1    = tbl.GetG4D("r_max1");
  G4double size_z    = tbl.GetG4D("size_z")*0.5;

  G4double r_min1 = tbl.Has("r_min1") ? tbl.GetG4D("r_min1") : 0.0;
  G4double r_min2 = tbl.Has("r_min2") ? tbl.GetG4D("r_min2") : 0.0;
  G4double r_max2 = tbl.Has("r_max2") ? tbl.GetG4D("r_max2") : 0.0;
  G4double phi_start = tbl.Has("phi_start") ? tbl.GetG4D("phi_start") : 0.0;
  G4double phi_delta = tbl.Has("phi_delta") ? tbl.GetG4D("phi_delta") : CLHEP::twopi;

  return new G4Cons(geo_id, 
		    r_min1, 
		    r_max1, 
		    r_min2,
		    r_max2,
		    size_z,
		    phi_start, 
		    phi_delta);
}

} // - namespace COSMIC

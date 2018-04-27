#include "GeoPara.hh"
#include "db/DBTable.hh"

namespace COSMIC {

GeoPara::GeoPara(DBTable table) {
  Construct(table);
}

G4VSolid* GeoPara::ConstructSolidVolume(DBTable tbl) {

  std::string geo_id = tbl.GetIndexName();

  // Now setup object
  std::vector<G4double> geo_size = tbl.GetVecG4D("size");
  G4double worldX = geo_size[0];
  G4double worldY = geo_size[1];
  G4double worldZ = geo_size[2];

  G4double alpha = tbl.Has("alpha") ? tbl.GetG4D("alpha") : 0.0;
  G4double theta = tbl.Has("theta") ? tbl.GetG4D("theta") : 0.0;
  G4double phi   =  tbl.Has("phi") ? tbl.GetG4D("phi") : 0.0;

  G4Para* geo_solid = new G4Para(geo_id,
				 0.5 * worldX,
				 0.5 * worldY,
				 0.5 * worldZ,
				 alpha,
				 theta,
				 phi);
  return geo_solid;
}

} // - namespace COSMIC

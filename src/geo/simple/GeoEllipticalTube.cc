#include "GeoEllipticalTube.hh"
#include "db/DBTable.hh"

namespace COSMIC {

GeoEllipticalTube::GeoEllipticalTube(DBTable table) {
  Construct(table);
}

G4VSolid* GeoEllipticalTube::ConstructSolidVolume(DBTable tbl) {
  std::cout << "Makign Elliptical Tube" << std::endl;
  std::string geo_id = tbl.GetIndexName();

  // Now setup object
  std::vector<G4double> geo_size = tbl.GetVecG4D("size");
  G4double worldX = geo_size[0];
  G4double worldY = geo_size[1];
  G4double worldZ = geo_size[2];
  G4EllipticalTube* geo_solid = new G4EllipticalTube(geo_id, 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);

  return geo_solid;
}

} // - namespace COSMIC

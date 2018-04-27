#include "GeoBox.hh"
#include "db/DBTable.hh"

namespace COSMIC {

GeoBox::GeoBox(DBTable table) {
  Construct(table);
}

G4VSolid* GeoBox::ConstructSolidVolume(DBTable tbl) {

  std::string geo_id = tbl.GetIndexName();

  // Now setup object
  std::vector<G4double> geo_size = tbl.GetVecG4D("size");  
  G4double worldX = geo_size[0];
  G4double worldY = geo_size[1];
  G4double worldZ = geo_size[2];
  G4Box* geo_solid = new G4Box(geo_id, 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);

  return geo_solid;
}

} // - namespace COSMIC

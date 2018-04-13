#include "GeoBox.hh"
#include "db/DBLink.hh"

namespace COSMIC {

GeoBox::GeoBox(DBLink* table) 
{
  Construct(table);
}

G4VSolid* GeoBox::ConstructSolidVolume(DBLink* tbl) {

  std::string geo_id = tbl->GetIndexName();

  // Now setup object
  std::vector<double> geo_size = tbl->GetVecD("size");
  G4double worldX = geo_size[0] * m;
  G4double worldY = geo_size[1] * m;
  G4double worldZ = geo_size[2] * m;
  G4Box* geo_solid = new G4Box(geo_id, 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);

  return geo_solid;
}

} // - namespace COSMIC
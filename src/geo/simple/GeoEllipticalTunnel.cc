#include "GeoEllipticalTunnel.hh"
#include "G4SubtractionSolid.hh"
#include "G4EllipticalTube.hh"
#include "G4Box.hh"
#include "db/DBTable.hh"

namespace COSMIC {

GeoEllipticalTunnel::GeoEllipticalTunnel(DBTable table) {
  Construct(table);
}

G4VSolid* GeoEllipticalTunnel::ConstructSolidVolume(DBTable tbl) {
  std::cout << "Making Elliptical Tunnel" << std::endl;
  std::string geo_id = tbl.GetIndexName();

  // Now setup object
  std::vector<G4double> geo_size = tbl.GetVecG4D("size");
  G4double worldX = geo_size[0];
  G4double worldY = geo_size[1];
  G4double worldZ = geo_size[2];
  std::string geo_id_tube = geo_id + std::string("_tube");
  G4EllipticalTube* geo_solid = new G4EllipticalTube(geo_id_tube, 0.5 * worldX, 0.5 * worldY, 0.5 * worldZ);

  // Add subtraction solid here for the floor
  std::string geo_id_floor = geo_id + std::string("_floor");
  G4Box* subtract_floor = new G4Box(geo_id_floor, 0.5 * worldX, worldY, worldZ);

  G4VSolid* tunnel  = new G4SubtractionSolid(geo_id, geo_solid, subtract_floor, 0, G4ThreeVector(-0.5*worldX,0.0,0.0));

  return tunnel;
}

} // - namespace COSMIC

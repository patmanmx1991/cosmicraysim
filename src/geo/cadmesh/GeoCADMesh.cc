#ifdef __USE_CADMESH__
#include "GeoCADMesh.hh"

#include "CADMesh.hh"

#include "db/DBTable.hh"

namespace COSMIC {

GeoCADMesh::GeoCADMesh(DBTable table) {
  Construct(table);
}

G4VSolid* GeoCADMesh::ConstructSolidVolume(DBTable tbl) {

  // Get Table Inputs
  std::string filename = tbl.GetS("file");

  G4double scale = mm;
  if (tbl.Has("scale")) scale = tbl.GetG4D("scale");
      
  std::vector<G4double> offset = std::vector<G4double>(3,0.0);
  if (tbl.Has("offset")) tbl.GetVecG4D("offset");

  bool reverse = false;
  if (tbl.Has("reverse")) reverse = tbl.GetB("reverse");

  // CAD model rotation.
  G4RotationMatrix * rot = new G4RotationMatrix();
  rot->rotateZ(90*deg);

  // Note that offset is applied to the points in mesh directly before placement.
  CADMesh * mesh = new CADMesh((char*) filename.c_str());
  mesh->SetScale(scale);
  mesh->SetOffset(G4ThreeVector(offset[0], offset[1], offset[2]));
  mesh->SetReverse(reverse);

  G4VSolid* geo_solid = mesh->TessellatedMesh();
  return geo_solid;
}

} // - namespace COSMIC
#endif // __USE_CADMESH__
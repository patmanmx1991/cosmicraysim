#include "GeoSolid.hh"

#include "GeoObject.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "GeoUtils.hh"
#include "sd/DetectorManager.hh"

namespace COSMIC {


void GeoSolid::Construct(DBLink* table) {
  fTable  = table;
  fName = fTable->GetIndexName();

  // Get mother
  fMotherLogical = GEO::GetMotherLogicalFromStore(fTable);

  // Construct Solid
  fSolid = ConstructSolidVolume(fTable);

  // Construct Logical Volume
  fLogical = ConstructLogicalVolume(fTable, fSolid);

  // Construct Physical Volume
  fPhysical = ConstructPhysicalVolume(fTable, fMotherLogical, fLogical);

  // Construct Sensitive Detector
  // fSensitive = ConstructSensitiveDetector(fTable, fLogical);
}


G4LogicalVolume* GeoSolid::ConstructLogicalVolume(DBLink* table, G4VSolid* solid) {
  G4NistManager* nist = G4NistManager::Instance();
  std::string name = table->GetIndexName();

  G4Material* geo_material = NULL;
  if (table->Has("material")) {
    std::string geo_matname = table->GetS("material");
    geo_material = nist->FindOrBuildMaterial(geo_matname);
  }
  G4LogicalVolume* geo_logic = new G4LogicalVolume(solid, geo_material, name);


  // Optional visualization parts
  G4VisAttributes *vis = new G4VisAttributes();
  if (table->Has("color")) {
    std::vector<double> color = table->GetVecD("color");
    if (color.size() == 3) // RGB
      vis->SetColour(G4Colour(color[0], color[1], color[2]));
    else if (color.size() == 4) // RGBA
      vis->SetColour(G4Colour(color[0], color[1], color[2], color[3]));
  }

  if (table->Has("optimize")) {
    int optimize = table->GetI("optimize");
    if (optimize == 0) geo_logic->SetOptimisation(false);
  }

  if (table->Has("drawstyle")) {
    std::string drawstyle = table->GetS("drawstyle");
    if (drawstyle == "wireframe")
      vis->SetForceWireframe(true);
    else if (drawstyle == "solid")
      vis->SetForceSolid(true);
    else throw;
  }

  if (table->Has("force_auxedge")) {
    int force_auxedge = table->GetI("force_auxedge");
    vis->SetForceAuxEdgeVisible(force_auxedge == 1);
  }

  geo_logic->SetVisAttributes(vis);

  // Check for invisible flag last
  if (table->Has("invisible")) {
    int invisible = table->GetI("invisible");
    if (invisible == 1) {
      geo_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  return geo_logic;
}

G4VPhysicalVolume* GeoSolid::ConstructPhysicalVolume(DBLink* table, G4LogicalVolume* mother, G4LogicalVolume* logic) {
  std::string name = table->GetIndexName();

  // Setup orientation
  G4RotationMatrix *rotation = new G4RotationMatrix();
  if (fTable->Has("orientation")) {
    const std::vector<double> &orientvector = table->GetVecD("orientation");
    G4ThreeVector soliddir;
    double angle_y = 0;
    double angle_x = 0;
    double angle_z = 0;
    soliddir.set(orientvector[0], orientvector[1], orientvector[2]);
    soliddir = soliddir.unit();
    angle_y = (-1.0) * atan2(soliddir.x(), soliddir.z());
    angle_x = atan2(soliddir.y(), sqrt(soliddir.x() * soliddir.x() + soliddir.z() * soliddir.z()));
    angle_z = atan2(-1 * soliddir.y() * soliddir.z(), soliddir.x());
    rotation->rotateY(angle_y);
    rotation->rotateX(angle_x);
    rotation->rotateZ(angle_z);
  }
  if (fTable->Has("rotation")) {
    const std::vector<double> &rotvector = table->GetVecD("rotation");
    rotation->rotateX(rotvector[0] * CLHEP::deg);
    rotation->rotateY(rotvector[1] * CLHEP::deg);
    rotation->rotateZ(rotvector[2] * CLHEP::deg);
  }

  // Setup positoin
  G4ThreeVector position(0.0, 0.0, 0.0);
  if (fTable->Has("position")) {
    const std::vector<double> &posvector = table->GetVecD("position");
    position.setX(posvector[0] * CLHEP::m);
    position.setY(posvector[1] * CLHEP::m);
    position.setZ(posvector[2] * CLHEP::m);
  }

  // Create Physical
  return new G4PVPlacement(rotation, position, logic, name,
                           mother, false /*?*/, 0 /*?*/);
}

void GeoSolid::ConstructSensitive() {
  fSensitive = ConstructSensitiveDetector(fTable, fLogical);
}

G4VSensitiveDetector* GeoSolid::ConstructSensitiveDetector(DBLink* table, G4LogicalVolume* logic) {

  // See if has sensitive detector
  if (!table->Has("sensitive")) return 0;

  // If it does then create a new SD object
  std::string sensitive = table->GetS("sensitive");
  G4VSensitiveDetector* sd = DetectorObjectFactory::CreateSD(sensitive);

  // Register the SD with the Geant4 SD Manager and our own
  G4SDManager::GetSDMpointer()->AddNewDetector(sd);
  DetectorManager::Get()->RegisterSensitiveDetector(table->GetS("index") + "_" + sensitive, sd);
  logic->SetSensitiveDetector(sd);

  // Return the sensitive detector for this volume
  return sd;
}





} // namespace COSMIC
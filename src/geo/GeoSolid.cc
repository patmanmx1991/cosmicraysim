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
#include "analysis/VDetector.hh"
#include "analysis/Analysis.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"
#include "physics/PhysicsFactory.hh"

#include "materials/MaterialManager.hh"

namespace COSMIC {

GeoSolid::GeoSolid()
{
}

void GeoSolid::Construct(DBTable table)
{
  SetID(table.GetIndexName());
  SetType(table.GetS("type"));

  // Get mother
  fMotherLogical = GEO::GetMotherLogicalFromStore(table);

  // Construct Solid
  fSolid = ConstructSolidVolume(table);

  // Construct Logical Volume
  fLogical = ConstructLogicalVolume(table, fSolid);

  // Construct Physical Volume
  fPhysical = ConstructPhysicalVolume(table, fMotherLogical, fLogical);

  // Construct Sensitive Detector
  fSensitive = ConstructSensitiveDetector(table, fLogical, fPhysical);
}


G4LogicalVolume* GeoSolid::ConstructLogicalVolume(DBTable table, G4VSolid* solid) {
  // G4NistManager* nist = G4NistManager::Instance();
  std::string name = table.GetIndexName();

  G4Material* geo_material = NULL;
  if (table.Has("material")) {
    std::string geo_matname = table.GetS("material");
    geo_material = MaterialFactory::GetMaterial(geo_matname);
  }
  G4LogicalVolume* geo_logic = new G4LogicalVolume(solid, geo_material, name);


  // Optional visualization parts
  G4VisAttributes *vis = new G4VisAttributes();
  if (table.Has("color")) {
    std::vector<double> color = table.GetVecD("color");
    if (color.size() == 3) // RGB
      vis->SetColour(G4Colour(color[0], color[1], color[2]));
    else if (color.size() == 4) // RGBA
      vis->SetColour(G4Colour(color[0], color[1], color[2], color[3]));
  }

  if (table.Has("optimize")) {
    int optimize = table.GetI("optimize");
    if (optimize == 0) geo_logic->SetOptimisation(false);
  }

  if (table.Has("drawstyle")) {
    std::string drawstyle = table.GetS("drawstyle");
    if (drawstyle == "wireframe")
      vis->SetForceWireframe(true);
    else if (drawstyle == "solid")
      vis->SetForceSolid(true);
    else throw;
  }

  if (table.Has("force_auxedge")) {
    int force_auxedge = table.GetI("force_auxedge");
    vis->SetForceAuxEdgeVisible(force_auxedge == 1);
  }

  geo_logic->SetVisAttributes(vis);

  // Check for invisible flag last
  if (table.Has("invisible")) {
    int invisible = table.GetI("invisible");
    if (invisible == 1) {
      geo_logic->SetVisAttributes(G4VisAttributes::Invisible);
    }
  }

  // If region then set to logical
  if (table.Has("cuts")) {
    G4Region* reg = new G4Region(name + "_" + table.GetS("cuts"));
    reg->SetProductionCuts( PhysicsFactory::LoadProductionCuts(table.GetS("cuts")) );
    reg->AddRootLogicalVolume(geo_logic);
  }

  return geo_logic;
}

G4VPhysicalVolume* GeoSolid::ConstructPhysicalPlacement(DBTable table, G4LogicalVolume* mother, G4LogicalVolume* logic) {

  std::string name = table.GetIndexName();

  // Setup orientation
  G4RotationMatrix *rotation = new G4RotationMatrix();
  if (table.Has("orientation")) {
    const std::vector<double> &orientvector = table.GetVecD("orientation");
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
  if (table.Has("rotation")) {
    const std::vector<double> &rotvector = table.GetVecD("rotation");
    rotation->rotateX(rotvector[0] * CLHEP::deg);
    rotation->rotateY(rotvector[1] * CLHEP::deg);
    rotation->rotateZ(rotvector[2] * CLHEP::deg);
  }

  // Setup positoin
  G4ThreeVector position(0.0, 0.0, 0.0);
  if (table.Has("position")) {
    const std::vector<double> &posvector = table.GetVecG4D("position");
    position.setX(posvector[0]);
    position.setY(posvector[1]);
    position.setZ(posvector[2]);
  }


  // Create Physical
  return new G4PVPlacement(rotation, position, logic, name,
                           mother, true /*?*/, 0 /*?*/);

}

G4VPhysicalVolume* GeoSolid::ConstructPhysicalReplica(DBTable table, G4LogicalVolume* /*mother*/, G4LogicalVolume* /*logic*/) {

  std::string volume_name = table.GetIndexName();
  G4VPhysicalVolume *pv = NULL;


  // int replicas = table.GetI("replicas");
  // string axis_str = table.GetS("replica_axis");
  // EAxis axis = kXAxis;

  // if (axis_str == "x")
  //   axis = kXAxis;
  // else if (axis_str == "y")
  //   axis = kYAxis;
  // else if (axis_str == "z")
  //   axis = kZAxis;
  // else if (axis_str == "rho")
  //   axis = kRho;
  // else if (axis_str == "phi")
  //   axis = kPhi;

  // G4double replica_spacing = table.GetD("replica_spacing") * CLHEP::mm;
  // pv = new G4PVReplica(volume_name, logic, mother, axis, replicas, replica_spacing);

  return pv;
}


G4VPhysicalVolume* GeoSolid::ConstructPhysicalParametrisation(DBTable table, G4LogicalVolume* /*mother*/, G4LogicalVolume* /*logic*/) {

  std::string volume_name = table.GetIndexName();
  G4VPhysicalVolume *pv = NULL;

  // This is a bit harder to do nested parametrisation.
  // Could do it in composite but would be hard to assign sensitive detectors.
  // Maybe thats fine? Need to make sensitive detectors record copyNo so that they account for a layer.
  // Also need to make the Drift Chamber positions do the same really....

  //

  // // std::string parametrisation = table.GetS("parametrisation");
  // // G4VPVParametrised* pr = GeoManager::GetParametrisation(parametrisation);

  // // new G4VPVParametrised(volume_name, logic, mother, axis, nch, )


  return pv;
}



G4VPhysicalVolume* GeoSolid::ConstructPhysicalVolume(DBTable table, G4LogicalVolume* mother, G4LogicalVolume* logic) {
  std::string name = table.GetIndexName();

  int replicas = 0;
  if (table.Has("replicas")) replicas = table.GetI("replicas");

  std::string parametrisation = "";
  if (table.Has("parametrisation"))  parametrisation = table.GetS("parametrisation");

  // Check if its placement/replica/parametrised
  if (replicas > 0) {

    // Check not dodgy inputs
    if (!parametrisation.empty()) {
      std::cout << "Can't give parametrisation and replaces in volume!" << std::endl;
      throw;
    }
    return ConstructPhysicalReplica(table, mother, logic);

    // Parametrisation
  } else if (!parametrisation.empty()) {
    return ConstructPhysicalParametrisation(table, mother, logic);

    // Placement
  } else {
    return ConstructPhysicalPlacement(table, mother, logic);
  }

  return 0;
}




G4VSensitiveDetector* GeoSolid::ConstructSensitiveDetector(DBTable table, G4LogicalVolume* logic, G4VPhysicalVolume* vol) {

  // See if has sensitive detector
  if (!table.Has("sensitive")) return 0;

  // If it does then create a new SD object
  std::string sensitive = table.GetS("sensitive");

  // First check detector not already loaded
  std::string sdname = table.GetIndexName();
  VDetector* sd = Analysis::Get()->GetDetector(sdname, true);
  if (!sd){
    DBTable sdtbl = DB::Get()->GetTable("DETECTOR", sensitive);
    sdtbl.SetIndexName(sdname);
    sd = DetectorObjectFactory::CreateSD(sdtbl);
    Analysis::Get()->RegisterDetector(sd);
  }

  // Assign the logical volume
  sd->SetLogicalVolume(logic, vol);

  // Return the sensitive detector for this volume
  return sd;
}

} // namespace COSMIC

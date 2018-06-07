#include "AWEDriftChamber.hh"

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


#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4Square.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Box.hh"
#include "G4Polyline.hh"
#include "Math/Functor.h"

#include "db/ROOTHeaders.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "sd/DetectorManager.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"
#include "sd/LongDriftSD.hh"
#include "chance/AWEDriftSD.hh"
#include "chance/BristolRPCSD.hh"

namespace COSMIC {

// --------------------------------------------------------------------------
void AWEDriftChamber::Construct(DBTable table) {
  SetID(table.GetIndexName());
  SetType("awe_drift");
  std::string fName = table.GetIndexName();
  std::string rootname = table.GetIndexName() + "_lvcont";
  std::string fMotherName = table.GetS("mother");

  // --------------------------------------------------------------------------
  // Composite is just a box of a given size with extra copper boxes above and below.
  // Dimensions of the box are hardcoded for now. Can be modified later.
  G4double chambersize_x  = 180*cm;
  G4double chambersize_y  = 60*cm;
  G4double chambersize_z  = 6*cm;
  G4double edge_thickness = 0.015*mm;
  G4double top_thickness  = 0.002*mm;
  G4String edge_material  = "G4_AIR";
  G4String top_material   = "G4_AIR";
  G4String gas_material   = "AWE_GASMIX1";
  G4String box_material   = "G4_AIR";

  // --------------------------------------------------------------------------
  // Make Container Box = AIR using position/rotation from main table
  std::vector<G4double> box_size_vector;
  box_size_vector.push_back( chambersize_x );
  box_size_vector.push_back( chambersize_y );
  box_size_vector.push_back( chambersize_z );
  table.Set("index",    rootname);
  table.Set("type",     "box");
  table.Set("size",     box_size_vector);
  table.Set("material", box_material);
  GeoBox* volume = new GeoBox(table);
  fSubObjects.push_back(volume);
  fLogical = volume->GetLogical();
  fPhysical = volume->GetPhysical();

  // --------------------------------------------------------------------------
  // Make inner gas detector
  std::vector<G4double> gas_size_vector;
  gas_size_vector.push_back( chambersize_x );
  gas_size_vector.push_back( chambersize_y - 2*edge_thickness );
  gas_size_vector.push_back( chambersize_z - 2*top_thickness );

  DBTable gasinfo = DBTable("GEO",fName);
  gasinfo.Set("type",     "box");
  gasinfo.Set("mother",    rootname);
  gasinfo.Set("material",  gas_material);
  gasinfo.Set("size",      gas_size_vector);
  gasinfo.Set("color",     DB::BlueColor());
  gasinfo.Set("drawstyle", "solid");
  gasinfo.Set("sensitive", "awe_drift_rec");
  GeoBox* gasobj = new GeoBox(gasinfo);
  fSubObjects.push_back( gasobj );

  // Get Sensitive from gas
  fSensitive = gasobj->GetSensitive();

  // --------------------------------------------------------------------------
  // Make all 4 edges

  // - Left side
  std::vector<G4double> edge_size_vector;
  edge_size_vector.push_back( chambersize_x );
  edge_size_vector.push_back( edge_thickness );
  edge_size_vector.push_back( chambersize_z - 2*top_thickness );

  std::vector<G4double> edge_pos_vector;
  edge_pos_vector.push_back( 0.0 );
  edge_pos_vector.push_back( 0.5*(chambersize_y - edge_thickness) );
  edge_pos_vector.push_back( 0.0 );

  DBTable edgeinfo = DBTable("GEO",rootname + "_edge");
  edgeinfo.Set("type",      "box");
  edgeinfo.Set("mother",    rootname);
  edgeinfo.Set("material",  edge_material);
  edgeinfo.Set("size",      edge_size_vector);
  edgeinfo.Set("position",  edge_pos_vector);
  edgeinfo.Set("color",     DB::RedColor());
  fSubObjects.push_back( new GeoBox(edgeinfo) );

  // - Right side
  edge_pos_vector[0] = 0.0;
  edge_pos_vector[1] = -0.5*(chambersize_y - edge_thickness);
  edge_pos_vector[2] = 0.0;
  edgeinfo.Set("position",  edge_pos_vector);
  fSubObjects.push_back( new GeoBox(edgeinfo) );

  // - Top side
  edge_size_vector[0] = chambersize_x;
  edge_size_vector[1] = chambersize_y;
  edge_size_vector[2] = top_thickness;
  edge_pos_vector[0] = 0.0;
  edge_pos_vector[1] = 0.0;
  edge_pos_vector[2] = +0.5*(chambersize_z - top_thickness);

  edgeinfo.Set("material",  top_material);
  edgeinfo.Set("size",      edge_size_vector);
  edgeinfo.Set("position",  edge_pos_vector);
  fSubObjects.push_back( new GeoBox(edgeinfo) );

  // - Bottom side
  edge_pos_vector[0] = 0.0;
  edge_pos_vector[1] = 0.0;
  edge_pos_vector[2] = -0.5*(chambersize_z - top_thickness);
  edgeinfo.Set("position",  edge_pos_vector);
  fSubObjects.push_back( new GeoBox(edgeinfo) );

}
// --------------------------------------------------------------------------
} // namespace COSMIC

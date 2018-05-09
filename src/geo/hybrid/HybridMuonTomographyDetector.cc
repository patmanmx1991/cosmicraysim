#include "HybridMuonTomographyDetector.hh"

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
#include "simple/GeoBox.hh"
#include "simple/GeoTubs.hh"
#include "sd/LongDriftSD.hh"
#include "sd/AWEDriftSD.hh"
#include "sd/BristolRPCSD.hh"

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
#include "db/ROOTHeaders.hh"
#include "G4Polyline.hh"
#include "Math/Functor.h"

namespace COSMIC {

// --------------------------------------------------------------------------
void HybridMuonTomographyDetector::Construct(DBTable table) {
  SetID(table.GetIndexName());
  SetType(table.GetS("type"));
  std::string fName = table.GetIndexName();
  std::string fMotherName = table.GetS("mother");

  // Load the temporary DB from geo file
  DB* tdb = DB::Get();
  tdb->CreateDataBase("hybriddet");
  tdb->SelectDataBase("hybriddet");
  std::string inputfile = DB::GetDataPath() + "/hybrid/muontomographydetector.geo";
  if (table.Has("input_file")) inputfile = table.GetS("input_file");
  tdb->LoadFile(inputfile);

  // Make the main logical volume (with mother and position/rotation overriden)
  DBTable voltable = DB::Get()->GetTable("GEO", "volume");
  voltable.SetTableName(fName);
  voltable.SetIndexName(fName + "_volume");
  voltable.Set("mother", fMotherName);

  if (table.Has("position"))
    voltable.Set("position", table.GetVecG4D("position"));
  if (table.Has("rotation"))
    voltable.Set("rotation", table.GetVecD("rotation"));

  GeoObject* volume = new GeoBox( voltable );
  fSubObjects.push_back(volume);
  fLogical = volume->GetLogical();
  fPhysical = volume->GetPhysical();

  // Make the scintillator plane
  std::cout << "GEO: --> Making scintillator panels" << std::endl;

  if (DB::Get()->HasTable("GEO","scintillator_top")){
    DBTable tbl = DB::Get()->GetTable("GEO", "scintillator_top");
    tbl.SetIndexName(fName + "_scintillator_top");
    tbl.Prefix("mother", fName + "_");
    fScintObjectTop = new GeoBox(tbl);
    fSubObjects.push_back(fScintObjectTop);
  }

  if (DB::Get()->HasTable("GEO","scintillator_bottom")){
    DBTable tbl = DB::Get()->GetTable("GEO", "scintillator_bottom");
    tbl.SetIndexName(fName + "_scintillator_bottom");
    tbl.Prefix("mother", fName + "_");
    fScintObjectBottom = new GeoBox(tbl);
    fSubObjects.push_back(fScintObjectBottom);
  }

  // -----------------------------------------------------------------
  // Make the drift chamber array
  DBTable postable = DB::Get()->GetTable("GEO", "chamber_positions");
  std::vector<std::string> fields = postable.GetFields();

  std::cout << "GEO: --> Making chambers" << std::endl;
  // Loop over all entries, look for chamber_${i}
  for (uint i = 0; i < 18; i++) {
    std::string chname = "chamber_" + std::to_string(i);

    // skip non positions
    if (!postable.Has(chname)) continue;

    // Get position
    std::vector<double> posrot = postable.GetVecD(chname);
    std::vector<double> position = {posrot[0], posrot[1], posrot[2]};
    std::vector<double> rotation = {posrot[3], posrot[4], posrot[5]};

    // Create the target by overloading table
    DBTable chtemplate = DB::Get()->GetTable("GEO", "drift_chamber");
    chtemplate.SetIndexName(fName + "_" + chname);
    chtemplate.Prefix("mother", fName + "_");
    chtemplate.Set("position", position);
    chtemplate.Set("rotation", rotation);

    GeoBox* obj = new GeoBox(chtemplate);
    fSubObjects.push_back(obj);
    fDriftObjects.push_back(obj);
  }

  // -----------------------------------------------------------------
  // Make the RPCs above and below
  if (DB::Get()->HasTable("GEO","rpc_topx")){
    DBTable rpctemplate = DB::Get()->GetTable("GEO","rpc_topx");
    rpctemplate.SetIndexName(fName + "_rpc_topx");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCTopX = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCTopX);
  }

  if (DB::Get()->HasTable("GEO","rpc_topy")){
    DBTable rpctemplate = DB::Get()->GetTable("GEO","rpc_topy");
    rpctemplate.SetIndexName(fName + "_rpc_topy");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCTopY = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCTopY);
  }

  if (DB::Get()->HasTable("GEO","rpc_bottomx")){
    DBTable rpctemplate = DB::Get()->GetTable("GEO","rpc_bottomx");
    rpctemplate.SetIndexName(fName + "_rpc_bottomx");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCBottomX = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCBottomX);
  }

  if (DB::Get()->HasTable("GEO","rpc_bottomy")){
    DBTable rpctemplate = DB::Get()->GetTable("GEO","rpc_bottomy");
    rpctemplate.SetIndexName(fName + "_rpc_bottomy");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCBottomY = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCBottomY);
  }

  // -----------------------------------------------------------------
  // Register an HybridMuonTomographyProcessor to combine hits and return trigger
  // if (!table.Has("processor") or table.GetB("processor")) {
  //   std::cout << "GEO: --> Making processor" << std::endl;
  //   Analysis::Get()->RegisterProcessor(new HybridMuonTomographyProcessor(this));
  // }

  // Remove the temporary database.
  tdb->SelectDataBase("default");
  std::cout << "GEO: --> Created Hybrid Detector" << std::endl;
}
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
HybridMuonTomographyProcessor::HybridMuonTomographyProcessor(HybridMuonTomographyDetector* det) :
  VProcessor(det->GetID()), fSave(true)
{
  // fHybridDetector = det;

  // // Get Scintillator objects
  // G4VSensitiveDetector* sd = fHybridDetector->GetScintillatorTop()->GetSensitive();
  // fScintProcTop = new SimpleScintillatorProcessor(static_cast<SimpleScintillatorSD*>(sd), true);

  // sd = fHybridDetector->GetScintillatorBottom()->GetSensitive();
  // fScintProcBottom = new SimpleScintillatorProcessor(static_cast<SimpleScintillatorSD*>(sd), true);

  // // Get RPC Objects
  // G4VSensitiveDetector* sd = fHybridDetector->GetRPCTop()->GetSensitive();
  // fRPCProcTop = new BristolRPCProcessor(static_cast<BristolRPCSD*>(sd), true);

  // sd = fHybridDetector->GetRPCBottom()->GetSensitive();
  // fRPCProcBottom = new BristolRPCProcessor(static_cast<BristolRPCSD*>(sd), true);

  // // From each one get the sensitive and manually create a processor.
  // // This is super awkward. May need to rethink.
  // std::vector<GeoObject*> drifts = fHybridDetector->GetDriftObjects();
  // for (uint i = 0; i < drifts.size(); i++) {
  //   AWEDriftSD* sd = static_cast<AWEDriftSD*>(drifts[i]->GetSensitive());
  //   AWEDriftProcessor* pr = new AWEDriftProcessor(sd, true);
  //   fDriftChamberProcs.push_back( pr );
  // }

}

bool HybridMuonTomographyProcessor::BeginOfRunAction(const G4Run* run) {
  // std::string tableindex = GetID();
  // std::cout << "Registering HybridMuonTomographyProcessor NTuples " << tableindex << std::endl;

  // G4AnalysisManager* man = G4AnalysisManager::Instance();

  // fScintProcTop->BeginOfRunAction(run);
  // fScintProcBottom->BeginOfRunAction(run); 

  // fTopRPCProc->BeginOfRunAction(run);
  // fBottomRPCProc->BeginOfRunAction(run);

  // for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
  //   fDriftChamberProcs[i]->BeginOfRunAction(run);
  // }

  // return true;
}


bool HybridMuonTomographyProcessor::ProcessEvent(const G4Event* event) {

  // // Get the processor from the scintillator
  // fScintProcTop->Reset();
  // fScintProcTop->ProcessEvent(event);
  // fScintProcBottom->Reset();
  // fScintProcBottom->ProcessEvent(event);
  // bool hasvalue = fScintProcTop->HasInfo() || fScintProcBottom->HasInfo();
  // if (!hasvalue) {
  //   return false;
  // }

  // G4double averagetime = 0.0;
  // for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
  //   // Run our processing
  //   fDriftChamberProcs[i]->Reset();
  //   fDriftChamberProcs[i]->ProcessEvent(event);
  // }

  // // Now get top bottom RPC Info
  // fTopRPCProc->Reset();
  // fTopRPCProc->ProcessEvent(event);
  
  // fBottomRPCProc->Reset();
  // fBottomRPCProc->ProcessEvent(event);

  // return true;
}

// --------------------------------------------------------------------------
} // namespace COSMIC

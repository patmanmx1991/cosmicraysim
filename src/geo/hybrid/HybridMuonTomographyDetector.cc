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

  if (DB::Get()->HasTable("GEO", "scintillator_top")) {
    DBTable tbl = DB::Get()->GetTable("GEO", "scintillator_top");
    tbl.SetIndexName(fName + "_scintillator_top");
    tbl.Prefix("mother", fName + "_");
    fScintObjectTop = new GeoBox(tbl);
    fSubObjects.push_back(fScintObjectTop);
  }

  if (DB::Get()->HasTable("GEO", "scintillator_bottom")) {
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
  if (DB::Get()->HasTable("GEO", "rpc_topx")) {
    DBTable rpctemplate = DB::Get()->GetTable("GEO", "rpc_topx");
    rpctemplate.SetIndexName(fName + "_rpc_topx");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCTopX = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCTopX);
  }

  if (DB::Get()->HasTable("GEO", "rpc_topy")) {
    DBTable rpctemplate = DB::Get()->GetTable("GEO", "rpc_topy");
    rpctemplate.SetIndexName(fName + "_rpc_topy");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCTopY = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCTopY);
  }

  if (DB::Get()->HasTable("GEO", "rpc_bottomx")) {
    DBTable rpctemplate = DB::Get()->GetTable("GEO", "rpc_bottomx");
    rpctemplate.SetIndexName(fName + "_rpc_bottomx");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCBottomX = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCBottomX);
  }

  if (DB::Get()->HasTable("GEO", "rpc_bottomy")) {
    DBTable rpctemplate = DB::Get()->GetTable("GEO", "rpc_bottomy");
    rpctemplate.SetIndexName(fName + "_rpc_bottomy");
    rpctemplate.Prefix("mother", fName + "_");
    fRPCBottomY = new GeoBox(rpctemplate);
    fSubObjects.push_back(fRPCBottomY);
  }

  // -----------------------------------------------------------------
  //Register an HybridMuonTomographyProcessor to combine hits and return trigger
  if (!table.Has("processor") or table.GetB("processor")) {
    std::cout << "GEO: --> Making processor" << std::endl;
    Analysis::Get()->RegisterProcessor(new HybridMuonTomographyProcessor(this));
  }

  // Remove the temporary database.
  tdb->SelectDataBase("default");
  std::cout << "GEO: --> Created Hybrid Detector" << std::endl;
}
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
HybridMuonTomographyProcessor::HybridMuonTomographyProcessor(HybridMuonTomographyDetector* det) :
  VProcessor(det->GetID()), fSave(true)
{
  fHybridDetector = det;

  // Get Scintillator objects
  G4VSensitiveDetector* sd = fHybridDetector->GetScintillatorTop()->GetSensitive();
  fScintProcTop = new SimpleScintillatorProcessor(static_cast<SimpleScintillatorSD*>(sd), false);

  sd = fHybridDetector->GetScintillatorBottom()->GetSensitive();
  fScintProcBottom = new SimpleScintillatorProcessor(static_cast<SimpleScintillatorSD*>(sd), false);

  // Get RPC Objects
  sd = fHybridDetector->GetRPCTopX()->GetSensitive();
  fRPCProcTopX = new BristolRPCProcessor(static_cast<BristolRPCSD*>(sd), false);

  sd = fHybridDetector->GetRPCTopY()->GetSensitive();
  fRPCProcTopY = new BristolRPCProcessor(static_cast<BristolRPCSD*>(sd), false);

  sd = fHybridDetector->GetRPCBottomX()->GetSensitive();
  fRPCProcBottomX = new BristolRPCProcessor(static_cast<BristolRPCSD*>(sd), false);

  sd = fHybridDetector->GetRPCBottomY()->GetSensitive();
  fRPCProcBottomY = new BristolRPCProcessor(static_cast<BristolRPCSD*>(sd), false);

  // From each one get the sensitive and manually create a processor.
  // This is super awkward. May need to rethink.
  std::vector<GeoObject*> drifts = fHybridDetector->GetDriftObjects();
  for (uint i = 0; i < drifts.size(); i++) {
    AWEDriftSD* sdd = static_cast<AWEDriftSD*>(drifts[i]->GetSensitive());
    AWEDriftProcessor* pr = new AWEDriftProcessor(sdd, false);
    fDriftChamberProcs.push_back( pr );
  }

}

bool HybridMuonTomographyProcessor::BeginOfRunAction(const G4Run* /*run*/) {
  std::string tableindex = GetID();
  std::cout << "Registering HybridMuonTomographyProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  ResetVariables();

  fScintTopEIndex = man ->CreateNtupleDColumn(tableindex + "_sct_te");
  fScintTopTIndex = man ->CreateNtupleDColumn(tableindex + "_sct_tt");
  fScintBottomEIndex = man ->CreateNtupleDColumn(tableindex + "_sct_be");
  fScintBottomTIndex = man ->CreateNtupleDColumn(tableindex + "_sct_bt");

  man ->CreateNtupleDColumn(tableindex + "_rpc_xx", fRPCHits_XX);
  man ->CreateNtupleDColumn(tableindex + "_rpc_xt", fRPCHits_XT);
  man ->CreateNtupleDColumn(tableindex + "_rpc_xz", fRPCHits_XZ);
  man ->CreateNtupleDColumn(tableindex + "_rpc_xe", fRPCHits_XE);

  man ->CreateNtupleDColumn(tableindex + "_rpc_yy", fRPCHits_YY);
  man ->CreateNtupleDColumn(tableindex + "_rpc_yt", fRPCHits_YT);
  man ->CreateNtupleDColumn(tableindex + "_rpc_yz", fRPCHits_YZ);
  man ->CreateNtupleDColumn(tableindex + "_rpc_ye", fRPCHits_YE);

  man ->CreateNtupleDColumn(tableindex + "_drift_xx", fDriftHits_XX);
  man ->CreateNtupleDColumn(tableindex + "_drift_xg", fDriftHits_XG);
  man ->CreateNtupleDColumn(tableindex + "_drift_xt", fDriftHits_XT);
  man ->CreateNtupleDColumn(tableindex + "_drift_xz", fDriftHits_XZ);
  man ->CreateNtupleDColumn(tableindex + "_drift_xe", fDriftHits_XE);

  man ->CreateNtupleDColumn(tableindex + "_drift_yy", fDriftHits_YY);
  man ->CreateNtupleDColumn(tableindex + "_drift_yg", fDriftHits_YG);
  man ->CreateNtupleDColumn(tableindex + "_drift_yt", fDriftHits_YT);
  man ->CreateNtupleDColumn(tableindex + "_drift_yz", fDriftHits_YZ);
  man ->CreateNtupleDColumn(tableindex + "_drift_ye", fDriftHits_YE);

  return true;
}


bool HybridMuonTomographyProcessor::ProcessEvent(const G4Event* event) {

  // Reset all processors
  fScintProcTop->Reset();
  fScintProcBottom->Reset();
  fRPCProcTopX->Reset();
  fRPCProcTopY->Reset();
  fRPCProcBottomX->Reset();
  fRPCProcBottomY->Reset();
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
    fDriftChamberProcs[i]->Reset();
  }

  // Run event sub processing
  fScintProcTop->ProcessEvent(event);
  fScintProcBottom->ProcessEvent(event);
  fRPCProcTopX->ProcessEvent(event);
  fRPCProcTopY->ProcessEvent(event);
  fRPCProcBottomX->ProcessEvent(event);
  fRPCProcBottomY->ProcessEvent(event);
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
    fDriftChamberProcs[i]->ProcessEvent(event);
  }

  // TRIGGERING
  // Check at least one scinillator hit
  bool hasvalue = fScintProcTop->HasInfo() || fScintProcBottom->HasInfo();
  if (!hasvalue) {
    ResetVariables();
    return false;
  }
  ResetVariables();

  // Scint info processing
  G4AnalysisManager* man = G4AnalysisManager::Instance();

  man->FillNtupleDColumn(fScintTopEIndex, fScintProcTop->GetEnergy());
  man->FillNtupleDColumn(fScintTopTIndex, fScintProcTop->GetEnergy());
  man->FillNtupleDColumn(fScintBottomEIndex, fScintProcBottom->GetEnergy());
  man->FillNtupleDColumn(fScintBottomTIndex, fScintProcBottom->GetEnergy());

  // RPC Hit Processing
  if (fRPCProcTopX->HasInfo()) {
    fRPCHits_XX.push_back(fRPCProcTopX->GetWorldPosX()/ m);
    fRPCHits_XT.push_back(fRPCProcTopX->GetWorldPosTrueX()/ m);
    fRPCHits_XZ.push_back(fRPCProcTopX->GetWorldPosZ()/ m);
    fRPCHits_XE.push_back(fRPCProcTopX->GetWorldErrX()/ m);
  }

  if (fRPCProcBottomX->HasInfo()) {
    fRPCHits_XX.push_back(fRPCProcBottomX->GetWorldPosX()/ m);
    fRPCHits_XT.push_back(fRPCProcBottomX->GetWorldPosTrueX()/ m);
    fRPCHits_XZ.push_back(fRPCProcBottomX->GetWorldPosZ()/ m);
    fRPCHits_XE.push_back(fRPCProcBottomX->GetWorldErrX()/ m);
  }

  if (fRPCProcTopY->HasInfo()) {
    fRPCHits_YY.push_back(fRPCProcTopY->GetWorldPosY()/ m);
    fRPCHits_YT.push_back(fRPCProcTopY->GetWorldPosTrueY()/ m);
    fRPCHits_YZ.push_back(fRPCProcTopY->GetWorldPosZ()/ m);
    fRPCHits_YE.push_back(fRPCProcTopY->GetWorldErrY()/ m);
  }

  if (fRPCProcBottomY->HasInfo()) {
    fRPCHits_YY.push_back(fRPCProcBottomY->GetWorldPosY()/ m);
    fRPCHits_YT.push_back(fRPCProcBottomY->GetWorldPosTrueY()/ m);
    fRPCHits_YZ.push_back(fRPCProcBottomY->GetWorldPosZ()/ m);
    fRPCHits_YE.push_back(fRPCProcBottomY->GetWorldErrY()/ m);
  }

  // Drift Processing is more awkward.
  // Count up in 3s, switching between left and right.
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
    if (!fDriftChamberProcs[i]->HasInfo()) continue;
    if (i % 6 < 3) {
      fDriftHits_YY.push_back(fDriftChamberProcs[i]->GetWorldPosY() / m);
      fDriftHits_YG.push_back(fDriftChamberProcs[i]->GetGhostWorldPosY() / m );
      fDriftHits_YT.push_back(fDriftChamberProcs[i]->GetWorldPosTrueY() / m );
      fDriftHits_YZ.push_back(fDriftChamberProcs[i]->GetWorldPosZ() / m );
      fDriftHits_YE.push_back(fDriftChamberProcs[i]->GetErrY()/ m);
    } else {
      fDriftHits_XX.push_back(fDriftChamberProcs[i]->GetWorldPosX()/ m);
      fDriftHits_XG.push_back(fDriftChamberProcs[i]->GetGhostWorldPosX()/ m);
      fDriftHits_XT.push_back(fDriftChamberProcs[i]->GetWorldPosTrueX()/ m);
      fDriftHits_XZ.push_back(fDriftChamberProcs[i]->GetWorldPosZ()/ m);
      fDriftHits_XE.push_back(fDriftChamberProcs[i]->GetErrY()/ m);
    }
  }

  return true;
}

void HybridMuonTomographyProcessor::ResetVariables() {

  fScintTopE = -999.0;
  fScintTopT = -999.0;
  fScintBottomT = -999.0;
  fScintBottomE = -999.0;

  fRPCHits_XX.clear();
  fRPCHits_XT.clear();
  fRPCHits_XZ.clear();
  fRPCHits_XE.clear();

  fRPCHits_YY.clear();
  fRPCHits_YT.clear();
  fRPCHits_YZ.clear();
  fRPCHits_YE.clear();

  fDriftHits_XX.clear();
  fDriftHits_XT.clear();
  fDriftHits_XG.clear();
  fDriftHits_XZ.clear();
  fDriftHits_XE.clear();

  fDriftHits_YY.clear();
  fDriftHits_YT.clear();
  fDriftHits_YG.clear();
  fDriftHits_YZ.clear();
  fDriftHits_YE.clear();

}

// --------------------------------------------------------------------------
} // namespace COSMIC

#include "HybridMuonTomographyDetector.hh"

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
#include "sd/AWEDriftSD.hh"
#include "sd/BristolRPCSD.hh"
#include "geo/chance/AWEDriftChamber.hh"

namespace COSMIC {

// --------------------------------------------------------------------------
void HybridMuonTomographyDetector::Construct(DBTable table) {
  SetID(table.GetIndexName());
  SetType(table.GetS("type"));
  std::string fName = table.GetIndexName();
  std::string fMotherName = table.GetS("mother");

  // -----------------------------------------------------------------
  // Load the temporary DB from geo file
  DB* tdb = DB::Get();
  tdb->CreateDataBase("hybriddet");
  tdb->SelectDataBase("hybriddet");
  std::string inputfile = DB::GetDataPath() + "/hybrid/muontomographydetector.geo";
  if (table.Has("input_file")) inputfile = table.GetS("input_file");
  std::cout << "Loading input file : " << inputfile << std::endl;
  tdb->LoadFile(inputfile);


  // -----------------------------------------------------------------
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


  // -----------------------------------------------------------------
  // Make the scintillator array
  std::cout << "GEO: --> Making scintillator" << std::endl;

  DBTable postable = DB::Get()->GetTable("GEO", "scintillator_positions");
  std::vector<std::string> fields = postable.GetFields();

  // Loop over all entries, look for chamber_${i}
  for (uint i = 0; i < fields.size(); i++) {
    std::string chname = "scintillator_" + std::to_string(i);

    // skip non positions
    if (!postable.Has(chname)) continue;

    // Get position
    std::vector<double> posrot = postable.GetVecG4D(chname);
    std::vector<double> position = {posrot[0], posrot[1], posrot[2]};
    std::vector<double> rotation = {posrot[3], posrot[4], posrot[5]};

    // Create the target by overloading table
    DBTable chtemplate = DB::Get()->GetTable("GEO", "scintillator_template");
    chtemplate.SetIndexName(fName + "_" + chname);
    chtemplate.Prefix("mother", fName + "_");
    chtemplate.Set("position", position);
    chtemplate.Set("rotation", rotation);

    GeoBox* obj = new GeoBox(chtemplate);
    fSubObjects.push_back(obj);
    fScintObjects.push_back(obj);
  }


  // -----------------------------------------------------------------
  // Make the drift chamber array
  std::cout << "GEO: --> Making chambers" << std::endl;

  postable = DB::Get()->GetTable("GEO", "drift_positions");
  fields = postable.GetFields();

  // Loop over all entries, look for chamber_${i}
  for (uint i = 0; i < fields.size(); i++) {
    std::string chname = "chamber_" + std::to_string(i);

    // skip non positions
    if (!postable.Has(chname)) continue;

    // Get position
    std::vector<double> posrot = postable.GetVecG4D(chname);
    std::vector<double> position = {posrot[0], posrot[1], posrot[2]};
    std::vector<double> rotation = {posrot[3], posrot[4], posrot[5]};

    // Create the target by overloading table
    DBTable chtemplate = DBTable("GEO",fName + "_" + chname);
    chtemplate.Set("mother", fName + "_volume");
    chtemplate.Set("position_units", "m");
    chtemplate.Set("position", position);
    chtemplate.Set("rotation", rotation);

    AWEDriftChamber* obj = new AWEDriftChamber(chtemplate);
    fSubObjects.push_back(obj);
    fDriftObjects.push_back(obj);
  }


  // -----------------------------------------------------------------
  // Make the drift chamber array
  std::cout << "GEO: --> Making RPCs" << std::endl;

  postable = DB::Get()->GetTable("GEO", "rpc_positions");
  fields = postable.GetFields();

  // Loop over all entries, look for chamber_${i}
  for (uint i = 0; i < fields.size(); i++) {
    std::string chname = "rpc_" + std::to_string(i);

    // skip non positions
    if (!postable.Has(chname)) continue;

    // Get position
    std::vector<double> posrot = postable.GetVecG4D(chname);
    std::vector<double> position = {posrot[0], posrot[1], posrot[2]};
    std::vector<double> rotation = {posrot[3], posrot[4], posrot[5]};

    // Create the target by overloading table

    DBTable chtemplate = DB::Get()->GetTable("GEO", "rpc_template");
    chtemplate.SetIndexName(fName + "_" + chname);
    chtemplate.Prefix("mother", fName + "_");
    chtemplate.Set("position", position);
    chtemplate.Set("rotation", rotation);

    GeoBox* obj = new GeoBox(chtemplate);
    fSubObjects.push_back(obj);
    fRPCObjects.push_back(obj);
  }

  // -----------------------------------------------------------------
  //Register an HybridMuonTomographyProcessor to combine hits and return trigger
  std::cout << "GEO: --> Making processor" << std::endl;

  // Get the table of infoids and turn into a map
  std::map<std::string, std::string> hitinfo;
  DBTable infotable = DB::Get()->GetTable("INFO", "detectors");
  fields = infotable.GetFields();
  for (uint i = 0; i < fields.size(); i++) {
    hitinfo[fName + "_" + fields[i]] = infotable.GetS(fields[i]);
  }

  // Create new tomography processor using hit info table
  Analysis::Get()->RegisterProcessor(new HybridMuonTomographyProcessor(this, hitinfo));

  // Remove the temporary database.
  tdb->SelectDataBase("default");
  std::cout << "GEO: --> Created Hybrid Detector" << std::endl;
}
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
HybridMuonTomographyProcessor::HybridMuonTomographyProcessor(HybridMuonTomographyDetector* det, std::map<std::string, std::string>& hitinfo) :
  VProcessor(det->GetID()), fSave(true)
{
  fHybridDetector = det;

  // From each scint get the sensitive and manually create a processor.
  std::vector<GeoObject*> scints = fHybridDetector->GetScintObjects();
  for (uint i = 0; i < scints.size(); i++) {
    SimpleScintillatorSD* sdd = static_cast<SimpleScintillatorSD*>(scints[i]->GetSensitive());
    SimpleScintillatorProcessor* pr = new SimpleScintillatorProcessor(sdd, false);
    fScintProcs.push_back( pr );
  }

  // From each RPC get the sensitive and manually create a processor.
  std::vector<GeoObject*> rpcs = fHybridDetector->GetRPCObjects();
  for (uint i = 0; i < rpcs.size(); i++) {
    BristolRPCSD* sdd = static_cast<BristolRPCSD*>(rpcs[i]->GetSensitive());
    BristolRPCProcessor* pr = new BristolRPCProcessor(sdd, false);
    fRPCProcs.push_back( pr );

    // Get Hit Info (e.g. X vs Y)
    std::string id = pr->GetID();
    // std::cout << "Getting hit info for " << id << " " << hitinfo[id] << std::endl;
    int info = ConvertHitInfo( hitinfo[id] );
    fRPCHitInfo.push_back( info );
  }

  // From each drift get the sensitive and manually create a processor.
  std::vector<GeoObject*> drifts = fHybridDetector->GetDriftObjects();
  for (uint i = 0; i < drifts.size(); i++) {
    AWEDriftSD* sdd = static_cast<AWEDriftSD*>(drifts[i]->GetSensitive());
    AWEDriftProcessor* pr = new AWEDriftProcessor(sdd, false);
    fDriftChamberProcs.push_back( pr );

    // Get Hit Info (e.g. X vs Y)
    std::string id = pr->GetID();
    int info = ConvertHitInfo( hitinfo[id] );
    if (info == -1){
      std::cout << "Cannot find hit info for : " << id << std::endl;
      throw;
    }
    fDriftHitInfo.push_back( info );
  }

}

int HybridMuonTomographyProcessor::ConvertHitInfo(std::string s){
  if (s.compare("x") == 0) return kHitInfoX;
  else if (s.compare("y") == 0) return kHitInfoY;
  else {
    std::cout << "Unknown hit info input : " << s << std::endl;
  }
  return -1;
}


bool HybridMuonTomographyProcessor::BeginOfRunAction(const G4Run* /*run*/) {
  std::string tableindex = GetID();
  std::cout << "Registering HybridMuonTomographyProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  ResetVariables();

  man ->CreateNtupleDColumn(tableindex + "_sct_e", fScintE);
  man ->CreateNtupleDColumn(tableindex + "_sct_t", fScintT);

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

  // Reset all variables
  ResetVariables();

  // Reset all processors
  for (uint i = 0; i < fScintProcs.size(); i++) {
    fScintProcs[i]->Reset();
  }
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
    fDriftChamberProcs[i]->Reset();
  }
  for (uint i = 0; i < fRPCProcs.size(); i++) {
    fRPCProcs[i]->Reset();
  }


  // Run event sub processing
  for (uint i = 0; i < fScintProcs.size(); i++) {
    fScintProcs[i]->ProcessEvent(event);
  }
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
    fDriftChamberProcs[i]->ProcessEvent(event);
  }
  for (uint i = 0; i < fRPCProcs.size(); i++) {
    fRPCProcs[i]->ProcessEvent(event);
  }


  // TRIGGERING
  // Check at least one scinillator hit
  bool hasvalue = false;
  for (uint i = 0; i < fScintProcs.size(); i++) {
    if (fScintProcs[i]->HasInfo()) hasvalue = true;
  }

  // If no scintillators loaded then just check if hits in RPC or Drift
  if (fScintProcs.empty()) {
    for (uint i = 0; i < fRPCProcs.size(); i++) {
      if (fRPCProcs[i]->HasInfo()) hasvalue = true;
    }
    for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
      if (fDriftChamberProcs[i]->HasInfo()) hasvalue = true;
    }
  }

  if (!hasvalue) {
    ResetVariables();
    return false;
  }


  // Saving
  // G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Loop over Scintillators and save info
  for (uint i = 0; i < fScintProcs.size(); i++) {
    if (!fScintProcs[i]->HasInfo()) continue;
    fScintE.push_back(fScintProcs[i]->GetEnergy() / MeV );
    fScintT.push_back(fScintProcs[i]->GetTime() / ns );
  }

  // Loop over all RPCs ans ave info
  for (uint i = 0; i < fRPCProcs.size(); i++) {

    // Skip unhit RPCs
    if (!fRPCProcs[i]->HasInfo()) continue;

    // Save relevant info
    if (fRPCHitInfo[i] == kHitInfoY) {
      fRPCHits_XX.push_back((fRPCProcs[i]->GetWorldPosY() + 250*mm/1.5)/(mm));
      fRPCHits_XT.push_back((fRPCProcs[i]->GetWorldPosTrueY() + 250*mm/1.5)/(mm));
      fRPCHits_XZ.push_back((fRPCProcs[i]->GetWorldPosZ())/(mm));
      fRPCHits_XE.push_back((fRPCProcs[i]->GetErrY())/(mm));

    } else if (fRPCHitInfo[i] == kHitInfoX){
      fRPCHits_YY.push_back((fRPCProcs[i]->GetWorldPosX() + 250*mm/1.5)/(mm));
      fRPCHits_YT.push_back((fRPCProcs[i]->GetWorldPosTrueX() + 250*mm/1.5)/(mm));
      fRPCHits_YZ.push_back((fRPCProcs[i]->GetWorldPosZ())/(mm));
      fRPCHits_YE.push_back((fRPCProcs[i]->GetErrY())/(mm));
    }
  }

  // Drift Processing is more awkward.
  // Count up in 3s, switching between left and right.
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
    
    // Skip unhit drifts
    if (!fDriftChamberProcs[i]->HasInfo()) continue;

    // Save relevent info
    if (fDriftHitInfo[i] == kHitInfoY) {
      fDriftHits_XX.push_back((fDriftChamberProcs[i]->GetWorldPosY() + 250*mm/1.5)/(mm));
      fDriftHits_XG.push_back((fDriftChamberProcs[i]->GetGhostWorldPosY() + 250*mm/1.5)/(mm));
      fDriftHits_XT.push_back((fDriftChamberProcs[i]->GetWorldPosTrueY() + 250*mm/1.5)/(mm));
      fDriftHits_XZ.push_back((fDriftChamberProcs[i]->GetWorldPosZ())/(mm));
      fDriftHits_XE.push_back((fDriftChamberProcs[i]->GetErrY())/(mm));

    } else if (fDriftHitInfo[i] == kHitInfoX) {
      fDriftHits_YY.push_back((fDriftChamberProcs[i]->GetWorldPosX() + 250*mm/1.5)/(mm));
      fDriftHits_YG.push_back((fDriftChamberProcs[i]->GetGhostWorldPosX() + 250*mm/1.5)/(mm));
      fDriftHits_YT.push_back((fDriftChamberProcs[i]->GetWorldPosTrueX() + 250*mm/1.5)/(mm));
      fDriftHits_YZ.push_back((fDriftChamberProcs[i]->GetWorldPosZ())/(mm));
      fDriftHits_YE.push_back((fDriftChamberProcs[i]->GetErrY())/(mm));
    }
  }

  return true;
}

void HybridMuonTomographyProcessor::ResetVariables() {

  fScintE.clear();
  fScintT.clear();

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

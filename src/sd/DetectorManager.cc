#include "sd/DetectorManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"

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


///Task5d.1: Don't forget to include header file of your sensitive detector
#include "db/DB.hh"
#include "db/DBTable.hh"
#include "db/DBLink.hh"
#include "MultiDetector.hh"
#include "TrueMuonTracker.hh"
#include "ScintillatorSD.hh"


#include <map>

namespace COSMIC {


G4VSensitiveDetector* DetectorObjectFactory::CreateSD(std::string sensitive) {


  // Retrieve the table that matches this sensitive
  DBLink* sd = DB::Get()->GetLink("DETECTOR",sensitive);
  std::string type = sd->GetS("type");

  
  if (type.compare("edep") == 0) return new MultiDetector(sd);
  else if (type.compare("truemuon") == 0) return new TrueMuonTracker(sd);
  else if (type.compare("scintillator") == 0) return new ScintillatorSD(sd);

  // Check we didn't get to here
  std::cout << "Failed to Create SD : " << type << std::endl;
  throw;  
  return 0;
}



G4VSensitiveDetector* DetectorObjectFactory::Construct(DBLink* table) {

  std::string type = table->GetS("type");

  if (type.compare("edep") == 0) return new MultiDetector(table);
  else if (type.compare("truemuon") == 0) return new TrueMuonTracker(table);
  else if (type.compare("scintillator") == 0) return new ScintillatorSD(table);

  // Check we didn't get to here
  std::cout << "Failed to Construct Geometry : " << type << std::endl;
  throw;  
  return 0;
}

// Static singleton object
DetectorManager *DetectorManager::fPrimary(0);

// Detector tables read 
DetectorManager::DetectorManager() {
}

// void DetectorManager::ConstructAll() {
//   std::vector<DBLink*> tables_clone = fDetTables;
//   std::vector<DBLink*>::iterator geo_iter = tables_clone.begin();
  
//   std::cout << "Constructing SensitiveDetector Objects : " << std::endl;
//   for (geo_iter = tables_clone.begin(); geo_iter != tables_clone.end(); geo_iter++) {
//     DBLink* geo_tab = (*geo_iter);
//     std::string geo_id = geo_tab->GetIndexName();
//     G4VSensitiveDetector* geo_obj = DetectorObjectFactory::Construct(geo_tab);
//     std::cout << " --> " << geo_id << std::endl;
//     fDetectors.push_back(geo_obj);
//     fNames.push_back(geo_id);
//     fRegistered.push_back(false);
//   }

//   return;
// }

bool DetectorManager::BeginOfRunAction(const G4Run* run, int& ntupleid, int& histid){
  bool saverow = false;
  for (int i = 0; i < fDetectors.size(); i++){
    if (!fRegistered[i]) continue;
    MultiDetector* multi = dynamic_cast<MultiDetector*>(fDetectors[i]);
    if (multi){ 
      saverow =  multi->BeginOfRunAction(run) || saverow;
      continue;
    } 
    DetectorObject* detobj = dynamic_cast<DetectorObject*>(fDetectors[i]);
    if (detobj){ 
      saverow =  detobj->BeginOfRunAction(run) || saverow;
      continue;
    } 
  }
  return saverow;
}

bool DetectorManager::RecordEvent(const G4Event* event){

  // Get the list of registered sensitive detectors


  // Loop over them and check none are our crazy inherited ones
  bool saverow = false;
  for (int i = 0; i < fDetectors.size(); i++){
    if (!fRegistered[i]) continue;
    // MultiDetector* multi = static_cast<MultiDetector*>(fDetectors[i]);
    // saverow =  multi->RecordEvent(event) || saverow; 
    MultiDetector* multi = dynamic_cast<MultiDetector*>(fDetectors[i]);
    if (multi){ 
      saverow =  multi->RecordEvent(event) || saverow;
      continue;
    } 
    DetectorObject* detobj = dynamic_cast<DetectorObject*>(fDetectors[i]);
    if (detobj){ 
      saverow =  detobj->RecordEvent(event) || saverow;
      continue;
    } 
  }
  return saverow;
}

// G4VSensitiveDetector* DetectorManager::RegisterSensitiveDetector(std::string name, G4LogicalVolume* logic){
//   for (int i = 0; i < fDetectors.size(); i++){
//     if (fNames[i].compare(name) != 0){ 
//       continue;
//     }
//     if (fRegistered[i]) {
//       std::cout << "Trying to dual register a SD!" << std::endl;
//       throw;
//     }
//     fRegistered[i] = true;
//     G4SDManager::GetSDMpointer()->AddNewDetector(fDetectors[i]);
//     logic->SetSensitiveDetector(fDetectors[i]);
//     return fDetectors[i];
//   }
//   std::cout << "FAILED!" << std::endl;
//   return 0;
// }

void DetectorManager::RegisterSensitiveDetector(std::string name, G4VSensitiveDetector* sd){
  fNames.push_back(name);
  fRegistered.push_back(true);
  fDetectors.push_back(sd);
}

}


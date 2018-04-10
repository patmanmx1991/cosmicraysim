#include "MultiDetector.hh"

namespace COSMIC{

MultiDetector::MultiDetector(DBLink* tbl)
: G4MultiFunctionalDetector(tbl->GetIndexName()) 
{
  fTable   = tbl;
  fType    = fTable->GetS("type");
  fTableIndex    = fTable->GetIndexName() + "_" + fType;
  fCollID = -1;
  
  // Setup Scorer
  G4VPrimitiveScorer* primitiv = NULL;
  primitiv = new G4PSEnergyDeposit(fType);

  // Register the scorer
  this->RegisterPrimitive(primitiv);
}

MultiDetector::~MultiDetector(){
}

bool MultiDetector::BeginOfRunAction(const G4Run* run){
  std::cout << "Registering MultiDetector NTuples " << fTableIndex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();
  fIndexEnergy = man->CreateNtupleDColumn(fTableIndex + "_E"); 

  return true;
}

int MultiDetector::GetCollectionID(){
  if (fCollID < 0) {
    G4String colname = fTable->GetIndexName() + "/" + fType;
    fCollID = G4SDManager::GetSDMpointer()->GetCollectionID(colname);
    if (fCollID == -1) throw;
  }
  return fCollID;
}

bool MultiDetector::RecordEvent(const G4Event* event){

  // Make sure collection ID valid
  GetCollectionID();

  // Get Hit Collection
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  G4HCofThisEvent* HCE = event->GetHCofThisEvent();
  if(!HCE) {
    return false;
  }
  
  // Get Hit map for this detector
  G4THitsMap<G4double>* evtMap = 
    static_cast<G4THitsMap<G4double>*>(HCE->GetHC(fCollID));
  std::map<G4int,G4double*>::iterator itr;

  // Loop over hitmap
  G4double totEdep = 0.;
  for (itr = evtMap->GetMap()->begin(); itr != evtMap->GetMap()->end(); itr++) {
    G4double edep = *(itr->second);
    totEdep += edep;
  }
  man->FillNtupleDColumn(fIndexEnergy, totEdep/MeV);  

  if (totEdep > 0) return true;
  else return false;
}




}
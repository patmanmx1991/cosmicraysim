#include "TrueMuonTracker.hh"
#include "analysis/Analysis.hh"
#include "db/DBLink.hh"

namespace COSMIC {

//------------------------------------------------------------------
TrueMuonTracker::TrueMuonTracker(DBLink* tbl):
  VDetector(tbl->GetIndexName(), "truemuon")
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  // std::cout << "TRUE MUON PROCESSOR : " << tbl->GetI("processor") << std::endl;
  if (!tbl->Has("processor") or tbl->GetI("processor") > 0) {
    Analysis::Get()->RegisterProcessor(new TrueMuonProcessor(this));
  }
}

TrueMuonTracker::TrueMuonTracker(std::string name, std::string id, bool autoprocess, bool autosave):
  VDetector(name, id)
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (autoprocess) {
    Analysis::Get()->RegisterProcessor(new TrueMuonProcessor(this, autosave));
  }
}

void TrueMuonTracker::ResetState() {
  VDetector::ResetState();
  fMuonTime = 0.0;
  fMuonMom = G4ThreeVector();
  fMuonPos = G4ThreeVector();
  fMuonPDG = -999;
  fMuonMomMag2 = -999.9;
  fTotEDep = 0.0;
}

G4bool TrueMuonTracker::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

  // Don't save tracks if no energy left in the detector
  G4double edep = step->GetTotalEnergyDeposit();
  fTotEDep += edep;
  if (edep <= 0.) return false;

  // Get only muons
  G4Track* track = step->GetTrack();
  int steppdg = track->GetParticleDefinition()->GetPDGEncoding();
  bool is_muon = ((steppdg == 13) || (steppdg == -13));
  if (!is_muon) return true;

  // Get the step+1 inside the detector
  G4StepPoint* steppoint = step->GetPostStepPoint();
  G4double steptime = steppoint->GetGlobalTime();
  G4ThreeVector steppos = steppoint->GetPosition();
  G4ThreeVector stepmom = track->GetMomentum();

  if (stepmom.mag2() > fMuonMomMag2) {
    fMuonPos = steppos;
    fMuonMom = stepmom;
    fMuonTime = steptime;
    fMuonPDG  = steppdg;
    fMuonMomMag2 = stepmom.mag2();
  }
  return true;
}


//------------------------------------------------------------------
TrueMuonProcessor::TrueMuonProcessor(TrueMuonTracker* trkr, bool autosave) :
  VProcessor(trkr->GetID()), fSave(autosave)
{
  fTracker = trkr;
}

bool TrueMuonProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = GetID();
  std::cout << "DET: Registering TrueMuonProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fMuonTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
  fMuonMomXIndex = man ->CreateNtupleDColumn(tableindex + "_px");
  fMuonMomYIndex = man ->CreateNtupleDColumn(tableindex + "_py");
  fMuonMomZIndex = man ->CreateNtupleDColumn(tableindex + "_pz");
  fMuonPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
  fMuonPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
  fMuonPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");
  fMuonPDGIndex  = man ->CreateNtupleIColumn(tableindex + "_pdg");

  return true;
}

bool TrueMuonProcessor::ProcessEvent(const G4Event* /*event*/) {
  
  // Register Trigger State
  fHasInfo = fTracker->GetTotEDep() > 0.0;
  fTime    = fTracker->GetMuonTime();
  fEnergy  = fTracker->GetTotEDep();
  
  // Set Ntuple to defaults
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->FillNtupleDColumn(fMuonTimeIndex, -999.);
  man->FillNtupleDColumn(fMuonMomXIndex, -999.);
  man->FillNtupleDColumn(fMuonMomYIndex, -999.);
  man->FillNtupleDColumn(fMuonMomZIndex, -999.);
  man->FillNtupleDColumn(fMuonPosXIndex, -999.);
  man->FillNtupleDColumn(fMuonPosYIndex, -999.);
  man->FillNtupleDColumn(fMuonPosZIndex, -999.);
  man->FillNtupleIColumn(fMuonPDGIndex,  -999 );

  if (fHasInfo) {
    // Fill muon vectors
    man->FillNtupleDColumn(fMuonTimeIndex, fTracker->GetMuonTime());
    man->FillNtupleDColumn(fMuonMomXIndex, fTracker->GetMuonMom().x() / MeV);
    man->FillNtupleDColumn(fMuonMomYIndex, fTracker->GetMuonMom().y() / MeV);
    man->FillNtupleDColumn(fMuonMomZIndex, fTracker->GetMuonMom().z() / MeV);
    man->FillNtupleDColumn(fMuonPosXIndex, fTracker->GetMuonPos().x() / m);
    man->FillNtupleDColumn(fMuonPosYIndex, fTracker->GetMuonPos().y() / m);
    man->FillNtupleDColumn(fMuonPosZIndex, fTracker->GetMuonPos().z() / m);
    man->FillNtupleIColumn(fMuonPDGIndex , fTracker->GetMuonPDG());
    return true;
  } else {
    return false;
  }
}

} // - namespace COSMIC

#include "TrueMuonTracker.hh"

namespace COSMIC {

TrueMuonTracker::TrueMuonTracker(DBLink* tbl)
  : DetectorObject(tbl)
{
  std::cout << "Creating new muon tracker " << std::endl;
  fTable   = tbl;
  fType    = fTable->GetS("type");
  fTableIndex    = fTable->GetIndexName() + "_" + fType;
}

TrueMuonTracker::~TrueMuonTracker() {
}

bool TrueMuonTracker::BeginOfRunAction(const G4Run* run) {
  std::cout << "Registering TrueMuonTracker NTuples " << fTableIndex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fMuonTimeIndex = man ->CreateNtupleDColumn(fTableIndex + "_t");
  fMuonMomXIndex = man ->CreateNtupleDColumn(fTableIndex + "_px");
  fMuonMomYIndex = man ->CreateNtupleDColumn(fTableIndex + "_py");
  fMuonMomZIndex = man ->CreateNtupleDColumn(fTableIndex + "_pz");
  fMuonPosXIndex = man ->CreateNtupleDColumn(fTableIndex + "_x");
  fMuonPosYIndex = man ->CreateNtupleDColumn(fTableIndex + "_y");
  fMuonPosZIndex = man ->CreateNtupleDColumn(fTableIndex + "_z");
  fMuonPDGIndex  = man ->CreateNtupleIColumn(fTableIndex + "_pdg");

  Reset();
  return true;
}

bool TrueMuonTracker::RecordEvent(const G4Event* event) {

  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->FillNtupleDColumn(fMuonTimeIndex, -999.);
  man->FillNtupleDColumn(fMuonMomXIndex, -999.);
  man->FillNtupleDColumn(fMuonMomYIndex, -999.);
  man->FillNtupleDColumn(fMuonMomZIndex, -999.);
  man->FillNtupleDColumn(fMuonPosXIndex, -999.);
  man->FillNtupleDColumn(fMuonPosYIndex, -999.);
  man->FillNtupleDColumn(fMuonPosZIndex, -999.);
  man->FillNtupleIColumn(fMuonPDGIndex, -999);



  if (fTotEDep > 0.) {
    // Fill muon vectors
    man->FillNtupleDColumn(fMuonTimeIndex, fMuonTime);
    man->FillNtupleDColumn(fMuonMomXIndex, fMuonMom.x() / MeV);
    man->FillNtupleDColumn(fMuonMomYIndex, fMuonMom.y() / MeV);
    man->FillNtupleDColumn(fMuonMomZIndex, fMuonMom.z() / MeV);
    man->FillNtupleDColumn(fMuonPosXIndex, fMuonPos.x() / m);
    man->FillNtupleDColumn(fMuonPosYIndex, fMuonPos.y() / m);
    man->FillNtupleDColumn(fMuonPosZIndex, fMuonPos.z() / m);
    man->FillNtupleIColumn(fMuonPDGIndex, fMuonPDG);
    Reset();
    return true;
  } else {
    Reset();
    return false;
  }
}

void TrueMuonTracker::Reset() {
  fMuonTime = 0.0;
  fMuonMom = G4ThreeVector();
  fMuonPos = G4ThreeVector();
  fMuonPDG = -999;
  fMuonMomMag2 = -999.9;
  fTotEDep = 0.0;
}


G4bool TrueMuonTracker::ProcessHits(G4Step* step, G4TouchableHistory* touch) {

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



}
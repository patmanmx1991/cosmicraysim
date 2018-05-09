#include "TargetMuonInOutTracker.hh"
#include "analysis/Analysis.hh"
#include "db/DBTable.hh"


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


namespace COSMIC {

//------------------------------------------------------------------
TargetMuonInOutTracker::TargetMuonInOutTracker(DBTable tbl):
  VDetector(tbl.GetIndexName(), "truemuon")
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  // std::cout << "TRUE MUON PROCESSOR : " << tbl->GetI("processor") << std::endl;
  if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
    Analysis::Get()->RegisterProcessor(new TargetMuonInOutProcessor(this));
  }
}

TargetMuonInOutTracker::TargetMuonInOutTracker(std::string name, std::string id, bool autoprocess, bool autosave):
  VDetector(name, id)
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (autoprocess) {
    Analysis::Get()->RegisterProcessor(new TargetMuonInOutProcessor(this, autosave));
  }
}

void TargetMuonInOutTracker::ResetState() {
  VDetector::ResetState();
  
  fFoundInStep  = false;
  fInPos = G4ThreeVector();
  fInMom = G4ThreeVector();

  fFoundOutStep = false;
  fOutPos = G4ThreeVector();
  fOutMom = G4ThreeVector();

}

G4bool TargetMuonInOutTracker::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

  // Get only muons
  G4Track* track = step->GetTrack();
  int steppdg = track->GetParticleDefinition()->GetPDGEncoding();
  bool is_muon = ((steppdg == 13) || (steppdg == -13));
  if (!is_muon) return false;

  // Get the first step inside the detector
  G4StepPoint* prestep = step->GetPreStepPoint();
  G4StepPoint* posstep = step->GetPostStepPoint();
  G4VSensitiveDetector* presd = prestep->GetSensitiveDetector();
  G4VSensitiveDetector* postsd = posstep->GetSensitiveDetector();

  // Set In point, first observed prestep point
  if (!fFoundInStep and presd and postsd){
    fInPos = prestep->GetPosition();
    fInMom = prestep->GetMomentum();
    fFoundInStep = true;
  }

  // Set Out point, last prestep point before leaving
  if (!fFoundOutStep and presd and !postsd) {
    fOutPos = prestep->GetPosition();
    fOutMom = prestep->GetMomentum();
    fFoundOutStep = true;
  }

  return true;
}


//------------------------------------------------------------------
TargetMuonInOutProcessor::TargetMuonInOutProcessor(TargetMuonInOutTracker* trkr, bool autosave) :
  VProcessor(trkr->GetID()), fSave(autosave)
{
  fTracker = trkr;
}

bool TargetMuonInOutProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = GetID();
  std::cout << "DET: Registering TargetMuonInOutProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fMuonInTimeIndex = man ->CreateNtupleDColumn(tableindex + "_in_t");
  fMuonInMomXIndex = man ->CreateNtupleDColumn(tableindex + "_in_px");
  fMuonInMomYIndex = man ->CreateNtupleDColumn(tableindex + "_in_py");
  fMuonInMomZIndex = man ->CreateNtupleDColumn(tableindex + "_in_pz");
  fMuonInPosXIndex = man ->CreateNtupleDColumn(tableindex + "_in_x");
  fMuonInPosYIndex = man ->CreateNtupleDColumn(tableindex + "_in_y");
  fMuonInPosZIndex = man ->CreateNtupleDColumn(tableindex + "_in_z");
  
  fMuonOutTimeIndex = man ->CreateNtupleDColumn(tableindex + "_out_t");
  fMuonOutMomXIndex = man ->CreateNtupleDColumn(tableindex + "_out_px");
  fMuonOutMomYIndex = man ->CreateNtupleDColumn(tableindex + "_out_py");
  fMuonOutMomZIndex = man ->CreateNtupleDColumn(tableindex + "_out_pz");
  fMuonOutPosXIndex = man ->CreateNtupleDColumn(tableindex + "_out_x");
  fMuonOutPosYIndex = man ->CreateNtupleDColumn(tableindex + "_out_y");
  fMuonOutPosZIndex = man ->CreateNtupleDColumn(tableindex + "_out_z");
  
  return true;
}

bool TargetMuonInOutProcessor::ProcessEvent(const G4Event* /*event*/) {
  
  // Register Trigger State
  fHasInfo = fTracker->GetMuonInTime() > 0.0 && fTracker->GetMuonOutTime() > 0.0;
  fTime    = 0.0;
  fEnergy  = 0.0;
  
  if (fHasInfo) {
    // Fill muon vectors
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fMuonInTimeIndex, fTracker->GetMuonInTime());
    man->FillNtupleDColumn(fMuonInMomXIndex, fTracker->GetMuonInMom().x() / MeV);
    man->FillNtupleDColumn(fMuonInMomYIndex, fTracker->GetMuonInMom().y() / MeV);
    man->FillNtupleDColumn(fMuonInMomZIndex, fTracker->GetMuonInMom().z() / MeV);
    man->FillNtupleDColumn(fMuonInPosXIndex, fTracker->GetMuonInPos().x() / m);
    man->FillNtupleDColumn(fMuonInPosYIndex, fTracker->GetMuonInPos().y() / m);
    man->FillNtupleDColumn(fMuonInPosZIndex, fTracker->GetMuonInPos().z() / m);

    man->FillNtupleDColumn(fMuonOutTimeIndex, fTracker->GetMuonOutTime());
    man->FillNtupleDColumn(fMuonOutMomXIndex, fTracker->GetMuonOutMom().x() / MeV);
    man->FillNtupleDColumn(fMuonOutMomYIndex, fTracker->GetMuonOutMom().y() / MeV);
    man->FillNtupleDColumn(fMuonOutMomZIndex, fTracker->GetMuonOutMom().z() / MeV);
    man->FillNtupleDColumn(fMuonOutPosXIndex, fTracker->GetMuonOutPos().x() / m);
    man->FillNtupleDColumn(fMuonOutPosYIndex, fTracker->GetMuonOutPos().y() / m);
    man->FillNtupleDColumn(fMuonOutPosZIndex, fTracker->GetMuonOutPos().z() / m);

    TargetMuonInOutProcessor::DrawEvent();

    return true;
  } else {
    // Set Ntuple to defaults
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fMuonInTimeIndex, -999.);
    man->FillNtupleDColumn(fMuonInMomXIndex, -999.);
    man->FillNtupleDColumn(fMuonInMomYIndex, -999.);
    man->FillNtupleDColumn(fMuonInMomZIndex, -999.);
    man->FillNtupleDColumn(fMuonInPosXIndex, -999.);
    man->FillNtupleDColumn(fMuonInPosYIndex, -999.);
    man->FillNtupleDColumn(fMuonInPosZIndex, -999.);

    man->FillNtupleDColumn(fMuonOutTimeIndex, -999.);
    man->FillNtupleDColumn(fMuonOutMomXIndex, -999.);
    man->FillNtupleDColumn(fMuonOutMomYIndex, -999.);
    man->FillNtupleDColumn(fMuonOutMomZIndex, -999.);
    man->FillNtupleDColumn(fMuonOutPosXIndex, -999.);
    man->FillNtupleDColumn(fMuonOutPosYIndex, -999.);
    man->FillNtupleDColumn(fMuonOutPosZIndex, -999.);
    return false;
  }
}

void TargetMuonInOutProcessor::DrawEvent(){
  // Draw Track if in interactive
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {
    G4ThreeVector muoninmom = fTracker->GetMuonInMom();
    G4ThreeVector muoninpos = fTracker->GetMuonInPos();

    G4Polyline polyline;
    polyline.push_back( muoninpos + 1.0 * m * muoninmom );
    polyline.push_back( muoninpos - 1.0 * m * muoninmom );

    G4Colour colour(1., 1., 0.);
    G4VisAttributes attribs(colour);
    polyline.SetVisAttributes(attribs);

    pVVisManager->Draw(polyline);

    G4ThreeVector muonoutmom = fTracker->GetMuonOutMom();
    G4ThreeVector muonoutpos = fTracker->GetMuonOutPos();

    G4Polyline polyline2;
    polyline2.push_back( muonoutpos + 1.0 * m * muonoutmom );
    polyline2.push_back( muonoutpos - 1.0 * m * muonoutmom );

    G4Colour colour2(0., 1., 1.);
    G4VisAttributes attribs2(colour2);
    polyline2.SetVisAttributes(attribs2);

    pVVisManager->Draw(polyline2);
  }
}

} // - namespace COSMIC

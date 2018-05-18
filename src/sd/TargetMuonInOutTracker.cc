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
#include "tomography/PoCAUtils.hh"

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
  fInTime = -999.0;

  fFoundOutStep = false;
  fOutPos = G4ThreeVector();
  fOutMom = G4ThreeVector();
  fOutTime = -999.0;

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
  if (!fFoundInStep and presd and postsd) {
    fInTime = prestep->GetGlobalTime();
    fInPos  = prestep->GetPosition();
    fInMom  = prestep->GetMomentum();
    fFoundInStep = true;
  }

  // Set Out point, last prestep point before leaving
  if (presd and !postsd) {
    fOutTime = prestep->GetGlobalTime();
    fOutPos  = prestep->GetPosition();
    fOutMom  = prestep->GetMomentum();
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


  fInMom.clear();
  fInMom.push_back(0.0);
  fInMom.push_back(0.0);
  fInMom.push_back(0.0);

  fOutMom.clear();
  fOutMom.push_back(0.0);
  fOutMom.push_back(0.0);
  fOutMom.push_back(0.0);

  fInPos.clear();
  fInPos.push_back(0.0);
  fInPos.push_back(0.0);
  fInPos.push_back(0.0);

  fOutPos.clear();
  fOutPos.push_back(0.0);
  fOutPos.push_back(0.0);
  fOutPos.push_back(0.0);

  fPoCA.clear();
  fPoCA.push_back(0.0);
  fPoCA.push_back(0.0);
  fPoCA.push_back(0.0);

  G4AnalysisManager* man = G4AnalysisManager::Instance();
  fMuonInTimeIndex = man ->CreateNtupleDColumn(tableindex + "_in_t");
  fMuonInPosXIndex = man ->CreateNtupleDColumn(tableindex + "_in_r", fInPos);
  fMuonInMomXIndex = man ->CreateNtupleDColumn(tableindex + "_in_p", fInMom);

  fMuonOutTimeIndex = man ->CreateNtupleDColumn(tableindex + "_out_t");
  fMuonOutMomXIndex = man ->CreateNtupleDColumn(tableindex + "_out_r", fOutPos);
  fMuonOutMomXIndex = man ->CreateNtupleDColumn(tableindex + "_out_p", fOutMom);

  fPoCAXIndex        = man ->CreateNtupleDColumn(tableindex + "_poca_r", fPoCA);
  fPoCAAngleIndex    = man ->CreateNtupleDColumn(tableindex + "_poca_th");
  fPoCADistanceIndex = man ->CreateNtupleDColumn(tableindex + "_poca_d");

  return true;
}

bool TargetMuonInOutProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Register Trigger State
  fHasInfo = fTracker->GetMuonInTime() > 0.0 || fTracker->GetMuonOutTime() > 0.0;
  fTime    = 0.0;
  fEnergy  = 0.0;
  if (!fSave) return fHasInfo;

  // Now save IN/OUT
  G4AnalysisManager* man = G4AnalysisManager::Instance();

  if (fTracker->GetMuonInTime() > 0.0) {
    fInPos[0] = fTracker->GetMuonInPos().x() / m;
    fInPos[1] = fTracker->GetMuonInPos().y() / m;
    fInPos[2] = fTracker->GetMuonInPos().z() / m;
    fInMom[0] = fTracker->GetMuonInMom().x() / MeV;
    fInMom[1] = fTracker->GetMuonInMom().y() / MeV;
    fInMom[2] = fTracker->GetMuonInMom().z() / MeV;
    man->FillNtupleDColumn(fMuonInTimeIndex, fTracker->GetMuonInTime());
  } else {
    fInPos[0] = -999.0;
    fInPos[1] = -999.0;
    fInPos[2] = -999.0;
    fInMom[0] = -999.0;
    fInMom[1] = -999.0;
    fInMom[2] = -999.0;
    man->FillNtupleDColumn(fMuonInTimeIndex, -999.);
  }

  if (fTracker->GetMuonOutTime() > 0.0) {
    fOutPos[0] = fTracker->GetMuonOutPos().x() / m;
    fOutPos[1] = fTracker->GetMuonOutPos().y() / m;
    fOutPos[2] = fTracker->GetMuonOutPos().z() / m;
    fOutMom[0] = fTracker->GetMuonOutMom().x() / MeV;
    fOutMom[1] = fTracker->GetMuonOutMom().y() / MeV;
    fOutMom[2] = fTracker->GetMuonOutMom().z() / MeV;
    man->FillNtupleDColumn(fMuonOutTimeIndex, fTracker->GetMuonOutTime());
  } else {
    fOutPos[0] = -999.0;
    fOutPos[1] = -999.0;
    fOutPos[2] = -999.0;
    fOutMom[0] = -999.0;
    fOutMom[1] = -999.0;
    fOutMom[2] = -999.0;
    man->FillNtupleDColumn(fMuonOutTimeIndex, -999.);
  }

  // Get POCA Information
  if (fTracker->GetMuonInTime() > 0.0 && fTracker->GetMuonInTime()) {

    G4ThreeVector poca;
    G4double angle;
    G4double distance;

    PoCAUtils::GetPoCAFromTracks(poca, angle, distance,
                                 fTracker->GetMuonInPos(), fTracker->GetMuonInMom(),
                                 fTracker->GetMuonOutPos(), fTracker->GetMuonOutMom());

    fPoCA[0] = poca[0] / m;
    fPoCA[1] = poca[1] / m;
    fPoCA[2] = poca[2] / m;
    man->FillNtupleDColumn(fPoCAAngleIndex, angle);
    man->FillNtupleDColumn(fPoCADistanceIndex, distance / m);

  } else {
    fPoCA[0] = -999.0;
    fPoCA[1] = -999.0;
    fPoCA[2] = -999.0;
    man->FillNtupleDColumn(fPoCAAngleIndex, -999.);
    man->FillNtupleDColumn(fPoCADistanceIndex, -999.);
  }

  TargetMuonInOutProcessor::DrawEvent();

  return fHasInfo;
}

void TargetMuonInOutProcessor::DrawEvent() {
  // Draw Track if in interactive
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {
    if (fTracker->GetMuonInTime() > 0.0) {
      G4Polyline polyline;
      polyline.push_back( fTracker->GetMuonInPos() - 0.5 * m * fTracker->GetMuonInMom().unit() );
      polyline.push_back( fTracker->GetMuonInPos() );
      // polyline.push_back( fTracker->GetMuonInPos() + 0.5 * m * fTracker->GetMuonInMom().unit() );
      G4Colour colour(1., 1., 1.);
      G4VisAttributes attribs(colour);
      polyline.SetVisAttributes(attribs);
      pVVisManager->Draw(polyline);
    }

    if (fTracker->GetMuonOutTime() > 0.0) {
      G4Polyline polyline2;
      // polyline2.push_back( fTracker->GetMuonOutPos() - 0.5 * m * fTracker->GetMuonOutMom().unit() );
      polyline2.push_back( fTracker->GetMuonOutPos() );
      polyline2.push_back( fTracker->GetMuonOutPos() + 0.5 * m * fTracker->GetMuonOutMom().unit() );
      G4Colour colour2(1., 1., 1.);
      G4VisAttributes attribs2(colour2);
      polyline2.SetVisAttributes(attribs2);
      pVVisManager->Draw(polyline2);
    }

    if (fTracker->GetMuonInTime() > 0.0 && fTracker->GetMuonOutTime() > 0.0) {
      G4Polyline polyline3;
      polyline3.push_back( fTracker->GetMuonInPos()  );
      polyline3.push_back( fTracker->GetMuonOutPos() );
      G4Colour colour3(1., 1., 1.);
      G4VisAttributes attribs3(colour3);
      polyline3.SetVisAttributes(attribs3);
      pVVisManager->Draw(polyline3);
    }
  }
}

} // - namespace COSMIC

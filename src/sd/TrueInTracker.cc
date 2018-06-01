#include "TrueInTracker.hh"
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
TrueInAbsorber::TrueInAbsorber(DBTable tbl):
  VDetector(tbl.GetIndexName(), "truein")
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
    Analysis::Get()->RegisterProcessor(new TrueInProcessor(this));
  }
}

TrueInAbsorber::TrueInAbsorber(std::string name, std::string id, bool autoprocess, bool autosave):
  VDetector(name, id)
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (autoprocess) {
    Analysis::Get()->RegisterProcessor(new TrueInProcessor(this, autosave));
  }
}

void TrueInAbsorber::ResetState() {
  VDetector::ResetState();

  particle_pdg.clear();
  particle_t.clear();
  particle_E.clear();
  particle_px.clear();
  particle_py.clear();
  particle_pz.clear();

}

G4bool TrueInAbsorber::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

  // Get the track
  G4Track* track = step->GetTrack();
  
  // Get the prestep
  G4StepPoint* prestep = step->GetPreStepPoint();

  particle_pdg.push_back(track->GetParticleDefinition()->GetPDGEncoding());
  particle_t  .push_back(prestep->GetGlobalTime() / ns);
  particle_E  .push_back(track->GetTotalEnergy()    / MeV);
  particle_px .push_back(track->GetMomentum()[0]    / MeV);
  particle_py .push_back(track->GetMomentum()[1]    / MeV);
  particle_pz .push_back(track->GetMomentum()[2]    / MeV);
  particle_x  .push_back(prestep->GetPosition()[0]  / mm);
  particle_y  .push_back(prestep->GetPosition()[1]  / mm);
  particle_z  .push_back(prestep->GetPosition()[2]  / mm);

  // The track is already in this sensitive volume so we can just kill it and save it.
  track->SetTrackStatus(fStopAndKill);

  return true;
}


//------------------------------------------------------------------
TrueInProcessor::TrueInProcessor(TrueInAbsorber* trkr, bool autosave) :
  VProcessor(trkr->GetID()), fSave(autosave)
{
  fTracker = trkr;
}

bool TrueInProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = GetID();
  std::cout << "DET: Registering TrueInProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->CreateNtupleDColumn(tableindex + "_t",   fTracker->GetParticleT());
  man->CreateNtupleDColumn(tableindex + "_E",   fTracker->GetParticleE());
  man->CreateNtupleIColumn(tableindex + "_pdg", fTracker->GetParticlePDG());
  man->CreateNtupleDColumn(tableindex + "_px",  fTracker->GetParticlePX());
  man->CreateNtupleDColumn(tableindex + "_py",  fTracker->GetParticlePY());
  man->CreateNtupleDColumn(tableindex + "_pz",  fTracker->GetParticlePZ());
  man->CreateNtupleDColumn(tableindex + "_x",   fTracker->GetParticleX());
  man->CreateNtupleDColumn(tableindex + "_y",   fTracker->GetParticleY());
  man->CreateNtupleDColumn(tableindex + "_z",   fTracker->GetParticleZ());

  return true;
}

bool TrueInProcessor::ProcessEvent(const G4Event* /*event*/) {
  fHasInfo = fTracker->GetNParticles() > 0;
  fTime    = 0.0;
  fEnergy  = 0.0;
  return fHasInfo;
}

} // - namespace COSMIC

#include "NeutronSD.hh"
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
NeutronSD::NeutronSD(DBTable tbl):
  VDetector(tbl.GetIndexName(), "neutron")
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  // std::cout << "TRUE Neutron PROCESSOR : " << tbl->GetI("processor") << std::endl;
  if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
    Analysis::Get()->RegisterProcessor(new NeutronProcessor(this));
  }
}

NeutronSD::NeutronSD(std::string name, std::string id, bool autoprocess, bool autosave):
  VDetector(name, id)
{
  std::cout << "DET: Creating new " << GetType()
            << " detector : " << GetID() << std::endl;

  // Set initial state
  ResetState();

  // By default also include the auto processor
  if (autoprocess) {
    Analysis::Get()->RegisterProcessor(new NeutronProcessor(this, autosave));
  }
}

void NeutronSD::ResetState() {
  VDetector::ResetState();
  fNeutronTime = 0.0;
  fNeutronPos = G4ThreeVector();
  fNeutronPDG = -999;
  fTotEDep = 0.0;
  fNNeutronHits = 0;
}

G4bool NeutronSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

  // Don't save tracks if no energy left in the detector
  G4double edep = step->GetTotalEnergyDeposit();
  fTotEDep += edep;
  if (edep <= 0.) return false;

  // Get only Neutrons
  G4Track* track = step->GetTrack();
  int steppdg = track->GetParticleDefinition()->GetPDGEncoding();

  // FOR DEBUG : std::cout << "StepPDG : " << steppdg << std::endl;
  bool is_Neutron = (steppdg == 2112);
  bool is_Alpha = (steppdg == 1000020040);
  if ( (!is_Neutron) || (!is_Alpha) ) return true;

  // Get the step+1 inside the detector
  G4StepPoint* steppoint = step->GetPostStepPoint();
  G4double steptime = steppoint->GetGlobalTime();
  G4ThreeVector steppos = steppoint->GetPosition();

  // Very inefficient lazy averaging
  fNeutronPos  *= fNNeutronHits;
  fNeutronTime *= fNNeutronHits;
  fNeutronPDG  *= fNNeutronHits;

  fNeutronPos  += steppos;
  fNeutronTime += steptime;
  fNeutronPDG  += steppdg;

  // Get new average
  fNNeutronHits++;
  fNeutronPos  /= fNNeutronHits;
  fNeutronTime /= fNNeutronHits;
  fNeutronPDG  /= fNNeutronHits;

  return true;
}


//------------------------------------------------------------------
NeutronProcessor::NeutronProcessor(NeutronSD* trkr, bool autosave) :
  VProcessor(trkr->GetID()), fSave(autosave)
{
  fTracker = trkr;
}

bool NeutronProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = GetID();
  std::cout << "DET: Registering NeutronProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fNeutronTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
  fNeutronEdepIndex = man ->CreateNtupleDColumn(tableindex + "_edep");
  fNeutronMultIndex = man ->CreateNtupleDColumn(tableindex + "_N");
  fNeutronPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
  fNeutronPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
  fNeutronPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");
  fNeutronPDGIndex  = man ->CreateNtupleIColumn(tableindex + "_pdg");

  return true;
}

bool NeutronProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Register Trigger State
  fHasInfo = fTracker->GetTotEDep() > 0.0;
  fTime    = fTracker->GetNeutronTime();
  fEnergy  = fTracker->GetTotEDep();

  if (fHasInfo) {
    // Fill Neutron vectors
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fNeutronTimeIndex, fTime);
    man->FillNtupleDColumn(fNeutronEdepIndex, fEnergy / MeV);
    man->FillNtupleDColumn(fNeutronMultIndex, fTracker->GetNeutronHitsNumber() );
    man->FillNtupleDColumn(fNeutronPosXIndex, fTracker->GetNeutronPos().x() / m);
    man->FillNtupleDColumn(fNeutronPosYIndex, fTracker->GetNeutronPos().y() / m);
    man->FillNtupleDColumn(fNeutronPosZIndex, fTracker->GetNeutronPos().z() / m);
    man->FillNtupleIColumn(fNeutronPDGIndex , fTracker->GetNeutronPDG());

    NeutronProcessor::DrawEvent();

    return true;
  } else {
    // Set Ntuple to defaults
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fNeutronTimeIndex, -999.);
    man->FillNtupleDColumn(fNeutronEdepIndex, -999.);
    man->FillNtupleDColumn(fNeutronPosXIndex, -999.);
    man->FillNtupleDColumn(fNeutronPosYIndex, -999.);
    man->FillNtupleDColumn(fNeutronPosZIndex, -999.);
    man->FillNtupleIColumn(fNeutronPDGIndex,  fTracker->GetNeutronPDG() );
    return false;
  }
}

void NeutronProcessor::DrawEvent(){
  // Draw Track if in interactive
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {
    G4ThreeVector Neutronpos = fTracker->GetNeutronPos();
    /*
        G4Polyline polyline;
        polyline.push_back( Neutronpos + 4.0 * m * Neutronmom );
        polyline.push_back( Neutronpos - 4.0 * m * Neutronmom );

        G4Colour colour(0., 0., 1.);
        G4VisAttributes attribs(colour);
        polyline.SetVisAttributes(attribs);

        pVVisManager->Draw(polyline);
    */
  }
}

} // - namespace COSMIC

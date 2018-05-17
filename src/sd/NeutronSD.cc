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
  fHits = 0;

  fNeutronEnergyMapped.clear();
  fNeutronKEMapped.clear();
  fNeutronTimeMapped.clear();
  fNeutronMomMapped.clear();
  fNeutronPosMapped.clear();

}

G4bool NeutronSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

  // Don't save tracks if no energy left in the detector
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep <= 0.) return false;

  // Get only Neutrons
  G4Track* track = step->GetTrack();
  int steppdg = track->GetParticleDefinition()->GetPDGEncoding();
  // G4ThreeVector steppos = steppoint->GetPosition();
  // G4ThreeVector stepmom =  track->GetMomentum();

  // FOR DEBUG :
  bool is_Neutron = (steppdg == 2112);
  bool is_Alpha = (steppdg == 1000020040);

  if ( !(is_Neutron||is_Alpha) ) return true;

  // We only want secondary particles in the detector
  if(step->GetTrack()->GetParentID() == 0) return true;

  // if(is_Neutron) if(edep>0) std::cout << " Neutron : " << edep << std::endl;
  // if(is_Alpha) if(edep>0) std::cout << " Alpha : " << edep << std::endl;

  G4StepPoint* steppoint = step->GetPostStepPoint();
  G4double steptime = steppoint->GetGlobalTime();

  // Get the unique id of this particle
  std::pair<G4int, G4int> this_particle_ids = std::make_pair( track->GetTrackID(), track->GetParentID() );

  // Check whether this particle already contributed to the energy deposited
  std::map< std::pair<G4int, G4int> , double >::iterator it = fNeutronEnergyMapped.find(this_particle_ids);

  bool already_exists = ( it != fNeutronEnergyMapped.end() );

  if(already_exists){
    // if it already exists then add the current energy deposited to this
    // - get the new value and reassign it to the current particle id
    fNeutronEnergyMapped[this_particle_ids] += edep;

    // Average the time of the hits by track and parent id
    fNeutronKEMapped[this_particle_ids] *= fHits;
    fNeutronTimeMapped[this_particle_ids] *= fHits;
    fNeutronMomMapped[this_particle_ids] *= fHits;
    fNeutronPosMapped[this_particle_ids] *= fHits;

    fNeutronKEMapped[this_particle_ids] += steppoint->GetKineticEnergy();
    fNeutronMomMapped[this_particle_ids] += track->GetMomentum();
    fNeutronPosMapped[this_particle_ids] += steppoint->GetPosition();
    fNeutronTimeMapped[this_particle_ids] += steppoint->GetGlobalTime();

    fHits++;
    fNeutronTimeMapped[this_particle_ids] /= fHits;
    fNeutronKEMapped[this_particle_ids] /= fHits;
    fNeutronMomMapped[this_particle_ids] /= fHits;
    fNeutronPosMapped[this_particle_ids] /= fHits;

  } else {
    fHits++;

    // if not add the ids to the map and initialize the value with the deposited energy
    fNeutronEnergyMapped[this_particle_ids] = edep;
    fNeutronKEMapped[this_particle_ids] = steppoint->GetKineticEnergy();
    fNeutronMomMapped[this_particle_ids] = steppoint->GetMomentumDirection();
    fNeutronPosMapped[this_particle_ids] = steppoint->GetPosition();
    fNeutronTimeMapped[this_particle_ids] = steppoint->GetGlobalTime();

  }

  // For DEBUG...
  // std::cout << std::endl << "StepPDG : " << steppdg << " Edep : " << edep << std::endl;
  // std::cout << "is_Neutron : " << is_Neutron << " is_Alpha : " << is_Alpha << std::endl;
  // std::cout << "StepPos : " << steppos  << std::endl << std::endl;

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
  fNeutronMultIndex = man ->CreateNtupleDColumn(tableindex + "_mult");
  fNeutronPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
  fNeutronPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
  fNeutronPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");
  fNeutronMomXIndex = man ->CreateNtupleDColumn(tableindex + "_px");
  fNeutronMomYIndex = man ->CreateNtupleDColumn(tableindex + "_py");
  fNeutronMomZIndex = man ->CreateNtupleDColumn(tableindex + "_pz");
  fNeutronKEIndex = man ->CreateNtupleDColumn(tableindex + "_ke");

  return true;
}

bool NeutronProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Register Trigger State
  fHasInfo = fTracker->GetMultiplicity() > 0.0;

  if (fHasInfo) {
    // Fill Neutron vectors
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fNeutronTimeIndex, fTracker->GetAverageTime());
    man->FillNtupleDColumn(fNeutronEdepIndex, fTracker->GetTotalEnergyDep() / MeV);
    man->FillNtupleDColumn(fNeutronMultIndex, fTracker->GetMultiplicity() );
    man->FillNtupleDColumn(fNeutronPosXIndex, fTracker->GetAveragePosition().x() );
    man->FillNtupleDColumn(fNeutronPosYIndex, fTracker->GetAveragePosition().y() );
    man->FillNtupleDColumn(fNeutronPosZIndex, fTracker->GetAveragePosition().z() );
    man->FillNtupleDColumn(fNeutronMomXIndex, fTracker->GetAverageMomentum().x() );
    man->FillNtupleDColumn(fNeutronMomYIndex, fTracker->GetAverageMomentum().y() );
    man->FillNtupleDColumn(fNeutronMomZIndex, fTracker->GetAverageMomentum().z() );
    man->FillNtupleDColumn(fNeutronKEIndex, fTracker->GetAverageKE()/MeV );

    NeutronProcessor::DrawEvent();

    return true;
  } else {
    // Set Ntuple to defaults
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fNeutronTimeIndex, -999);
    man->FillNtupleDColumn(fNeutronEdepIndex, -999);
    man->FillNtupleDColumn(fNeutronMultIndex, -999);
    man->FillNtupleDColumn(fNeutronMomXIndex, -999 );
    man->FillNtupleDColumn(fNeutronMomYIndex, -999 );
    man->FillNtupleDColumn(fNeutronMomZIndex, -999);
    man->FillNtupleDColumn(fNeutronKEIndex, -999 );
    return false;
  }
}

void NeutronProcessor::DrawEvent(){
  // Draw Track if in interactive
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {

      G4ThreeVector Neutronpos = fTracker->GetAveragePosition();

        G4Circle NeutronMarker(Neutronpos);

        G4Colour colour(1., 0., 0.);
        G4VisAttributes attribs(colour);
        NeutronMarker.SetVisAttributes(attribs);

        pVVisManager->Draw(NeutronMarker);

  }
}

} // - namespace COSMIC

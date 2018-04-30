#include "action/CosmicStackingAction.hh"

#include "G4Track.hh"
#include "G4NeutrinoE.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"

CosmicStackingAction::CosmicStackingAction()
{ }

CosmicStackingAction::~CosmicStackingAction()
{ }

G4ClassificationOfNewTrack
CosmicStackingAction::ClassifyNewTrack(const G4Track* track)
{
  //kill secondary neutrino
  if (track->GetParentID()>0) {
    if (track->GetDefinition() == G4NeutrinoE::NeutrinoE() ||
      track->GetDefinition() == G4Electron::Electron() || 
      track->GetDefinition() == G4Positron::Positron() )
    return fKill;
  }
  //otherwise, return what Geant4 would have returned by itself
  // else 
  return G4UserStackingAction::ClassifyNewTrack(track);
}


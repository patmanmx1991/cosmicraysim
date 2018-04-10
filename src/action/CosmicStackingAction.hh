#ifndef CosmicStackingAction_h
#define CosmicStackingAction_h 1

#include "G4UserStackingAction.hh"
#include "globals.hh"

/// Stacking action class : manage the newly generated particles
///
/// One wishes do not track secondary neutrino.Therefore one kills it 
/// immediately, before created particles will  put in a stack.

class CosmicStackingAction : public G4UserStackingAction
{
  public:
    CosmicStackingAction();
    virtual ~CosmicStackingAction();
     
    virtual G4ClassificationOfNewTrack ClassifyNewTrack(const G4Track*);        
};


#endif


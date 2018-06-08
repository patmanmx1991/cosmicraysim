#ifdef __USE_PUMAS__
//******************************************************************************
// PumasBackwardsGenerator.cc
//
// 1.00 JMV, LLNL, Jan-2007:  First version.
//******************************************************************************
//
#include "PumasBackwardsGenerator.hh"
#include "db/DB.hh"
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "pumas/PUMASInterface.hh"
/* Standard library includes */
#include <errno.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "pumas/pumas.h"
using namespace PUMASInterface;

namespace COSMIC {

//----------------------------------------------------------------------------//
PumasBackwardsGenerator::PumasBackwardsGenerator() 
{

  fSetupState = false;
  fKineticThreshold = 5E6;
  

  // Set exposure and nthrows
  fExposureTime = 0.0;
  fNThrows = 0;
  // Make a new processor
  // Analysis::Get()->SetFluxProcessor(new PumasBackwardFluxProcessor(this));
  Analysis::Get()->SetFluxProcessor(new PumasBackwardsProcessor(this));
}

//----------------------------------------------------------------------------//
PumasBackwardsGenerator::~PumasBackwardsGenerator()
{
}

void PumasBackwardsGenerator::Setup(){
  // Build the PUMAS Context
  pumas_context* context = PUMASInterface::BuildPUMAS();
  context->scheme = PUMAS_SCHEME_HYBRID;
  context->longitudinal = 0;
  context->kinetic_limit = fKineticThreshold;
  std::cout << "Built context : " << context << std::endl;
  fSetupState = true;
}


void PumasBackwardsGenerator::GeneratePrimaries(G4Event* anEvent)
{
  if (!fSetupState) Setup();

  std::cout << " Running throw : " << std::endl;
  // Throw an angle and an energy;
  double cos_theta = -1.0 + 2.0*PUMASInterface::uniform01(context);
  double sin_theta = -1.0 + 2.0*PUMASInterface::uniform01(context);
  double kf = 1000;
  double wf = 1;

  // Update the PUMAS_STATE
  struct pumas_state state = {.charge = -1.,
           .kinetic = kf,
            .weight = wf,
             .direction = { -sin_theta, 0., -cos_theta }
  };

  // Update vectors
  fMuonPos    = G4ThreeVector(0, 0, 0);
  fMuonDir    = G4ThreeVector(-sin_theta, 0., -cos_theta);
  fMuonEnergy = kf * MeV;
  fMuonTime   = 1*ns;

  std::cout << "Created : " << state.kinetic << std::endl;

  // While the muon is less than some threshold, project backwards
  while (state.kinetic < fKineticThreshold - FLT_EPSILON) {

    std::cout << "While" << std::endl;

    // Transport a step
    pumas_transport(context, &state);

    double val = medium2(context, &state, NULL);
    std::cout << "Checking position : " << val << std::endl;
    if (val <= 0) break;
  }

  std::cout << "Checkout " << std::endl;

  // Get a weight for the muon depending on its angle and momentum.
  const double wi = state.weight *
                    spectrum_gaisser(-state.direction[2], state.kinetic);

  std::cout << "Particle Weight : " << state.weight << std::endl;
  std::cout << "Final particle pos : " << state.position[0] << " "  << state.position[1] << " " << state.position[2] << std::endl;
  std::cout << "Final particle dir : " << state.direction[0] << " "  << state.direction[1] << " " << state.direction[2] << std::endl;

  // Set G4 Properties
  fParticleGun->SetParticleEnergy(fMuonEnergy);
  fParticleGun->SetParticleTime(fMuonTime);
  fParticleGun->SetParticleMomentumDirection(fMuonDir);
  fParticleGun->SetParticlePosition(fMuonPos);
  fParticleGun->GeneratePrimaryVertex(anEvent);

  std::cout << "Generating Particle : " << fMuonEnergy << " " << fMuonTime << std::endl;

}
//------------------------------------------------------------------



//------------------------------------------------------------------
PumasBackwardsProcessor::PumasBackwardsProcessor(PumasBackwardsGenerator* gen, bool autosave) :
  VFluxProcessor("pumas")
{
  fGenerator = gen;
}

bool PumasBackwardsProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  return true;
}

bool PumasBackwardsProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Register Trigger State
  fHasInfo = true;
  fTime    = 1.0;

  return true;
}

} // - namespace COSMIC
#endif // __USE_PUMAS__

#include "action/CosmicActionInitialization.hh"
#include "action/CosmicRunAction.hh"
#include "action/CosmicStackingAction.hh"
#include "flux/PrimaryGeneratorFactory.hh"
#include "physics/PhysicsFactory.hh"
#include "sd/DetectorConstruction.hh"

namespace COSMIC {
CosmicActionInitialization::CosmicActionInitialization()
 : G4VUserActionInitialization()
{}


CosmicActionInitialization::~CosmicActionInitialization()
{}


void CosmicActionInitialization::BuildForMaster() const
{
  SetUserAction(new CosmicRunAction);
}


void CosmicActionInitialization::Build() const
{
  SetUserAction(PrimaryGeneratorFactory::LoadGenerator());
  SetUserAction(new CosmicRunAction);
  SetUserAction(new CosmicStackingAction);
}  
}

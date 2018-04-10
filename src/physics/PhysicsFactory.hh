#ifndef __PhysicsFactory_HH__
#define __PhysicsFactory_HH__
#include "G4VModularPhysicsList.hh"

namespace COSMIC{
class DBLink;
namespace PhysicsFactory{
  G4VUserPhysicsList* LoadPhysicsList(DBLink* table=0);
} // - namespace PhysicsFactory
} // - namespace COSMIC

#endif
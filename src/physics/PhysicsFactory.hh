#ifndef __PhysicsFactory_HH__
#define __PhysicsFactory_HH__
#include "G4VModularPhysicsList.hh"

#include "db/DB.hh"
#include "db/DBLink.hh"


namespace COSMIC{
/// Physics List Creator Name Space
namespace PhysicsFactory{
  /// Function to return a given physics list by table definition
  /// 
  /// If no table provided, the global table definition is used.
  G4VUserPhysicsList* LoadPhysicsList(DBLink* table=0);
} // - namespace PhysicsFactory
} // - namespace COSMIC

#endif
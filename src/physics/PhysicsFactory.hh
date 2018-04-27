#ifndef __PhysicsFactory_HH__
#define __PhysicsFactory_HH__
#include "G4VModularPhysicsList.hh"
#include "G4Region.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"

namespace COSMIC{
/// Physics List Creator Name Space
namespace PhysicsFactory{
  /// Function to return a given physics list by table definition
  /// 
  /// If no table provided, the global table definition is used.
  G4VUserPhysicsList* LoadPhysicsList(DBTable tbl);
  G4VUserPhysicsList* LoadPhysicsList();

  G4Region* LoadRegion(DBTable tbl);

} // - namespace PhysicsFactory
} // - namespace COSMIC

#endif

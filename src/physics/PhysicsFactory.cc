#include "PhysicsFactory.hh"

#include "Shielding.hh"

#include "db/DB.hh"
#include "db/DBLink.hh"
#include "DefaultPhysics.hh"

namespace COSMIC {
G4VUserPhysicsList* PhysicsFactory::LoadPhysicsList(DBLink* table){

  // If none passed return the global list
  if (!table) table = DB::Get()->GetLink("GLOBAL","config");

  // Get physics list by horrible string comparison
  std::string physics = table->GetS("physics");
  if (physics.compare("shielding") == 0) return new Shielding();
  else if (physics.compare("default") == 0) return new DefaultPhysics();

  // if here then failed
  std::cout << "Could not find physics list : " << physics << std::endl;
  throw;
}

}
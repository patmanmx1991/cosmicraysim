#include "PhysicsFactory.hh"

#include "DefaultPhysics.hh"
#include "Shielding.hh"
#include "db/DB.hh"
#include "db/DBLink.hh"
namespace COSMIC {
G4VUserPhysicsList* PhysicsFactory::LoadPhysicsList(DBLink* table){

  // If none passed return the global list
  if (!table) table = DB::Get()->GetLink("GLOBAL","config");

  std::string physics = table->GetS("physics");
  if (physics.compare("shielding") == 0) return new Shielding();
  else if (physics.compare("default") == 0) return new DefaultPhysics();

  // if here then failed
  std::cout << "Could not find physics list : " << physics << std::endl;
  throw;
}

}
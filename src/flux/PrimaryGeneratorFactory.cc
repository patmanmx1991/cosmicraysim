#include "PrimaryGeneratorFactory.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "flux/ShuklaPrimaryGenerator.hh"
#include "flux/CRYPrimaryGenerator.hh"
#include "flux/PumasBackwardsGenerator.hh"

namespace COSMIC{

G4VUserPrimaryGeneratorAction* PrimaryGeneratorFactory::LoadGenerator(){
  // Get default config if requested
  DBTable tbl = DB::Get()->GetTable("GLOBAL","config");
  return LoadGenerator(tbl);
}

G4VUserPrimaryGeneratorAction* PrimaryGeneratorFactory::LoadGenerator(DBTable table){

  // Read Flux Table using horrible ugly string comparisons!
  std::string type = table.GetS("flux");
  std::cout << "FLX: Loading Primary Generator : " << type << std::endl;
  if (type.compare("shukla")==0) return new ShuklaPrimaryGenerator();
  if (type.compare("cry")==0)    return new CRYPrimaryGenerator();

  #ifdef __USE_PUMAS__
  if (type.compare("pumasback") ==0) return new PumasBackwardsGenerator();
  #endif
  
  // Check if onle made
  std::cout << "Generator Creation Failed!" << std::endl;
  throw;

  // Return NULL generator
  return 0;
}
} // - namespace COSMIC


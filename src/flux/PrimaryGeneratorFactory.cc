#include "PrimaryGeneratorFactory.hh"

#include "db/DB.hh"
#include "db/DBLink.hh"
#include "flux/ShuklaPrimaryGenerator.hh"
#include "flux/CRYPrimaryGenerator.hh"

namespace COSMIC{

G4VUserPrimaryGeneratorAction* PrimaryGeneratorFactory::LoadGenerator(DBLink* table){

  // Get Set of flux tables to check only one loaded.
  if (!table) table = DB::Get()->GetLink("GLOBAL","config");

  // Read Flux Table using horrible ugly string comparisons!
  std::string type = table->GetS("flux");
  std::cout << "FLX: Loading Primary Generator : " << type << std::endl;
  if (type.compare("shukla")==0) return new ShuklaPrimaryGenerator();
  if (type.compare("cry")==0)    return new CRYPrimaryGenerator();

  // Check if onle made
  std::cout << "Generator Creation Failed!" << std::endl;
  throw;

  // Return NULL generator
  return 0;
};

} // - namespace COSMIC


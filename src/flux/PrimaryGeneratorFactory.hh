#ifndef __COSMIC_PrimaryGeneratorFactory_HH__
#define __COSMIC_PrimaryGeneratorFactory_HH__

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Types.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "db/DBLink.hh"

namespace COSMIC{

/// Flux Factory function to load generators from table
namespace PrimaryGeneratorFactory {
  G4VUserPrimaryGeneratorAction* LoadGenerator(DBLink* table=0);
}

} // - namespace COSMIC
#endif



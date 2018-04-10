
#ifndef __PrimaryGeneratorFactory__HH__
#define __PrimaryGeneratorFactory__HH__

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4Types.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

#include "db/DBLink.hh"

namespace COSMIC{
namespace PrimaryGeneratorFactory {
  G4VUserPrimaryGeneratorAction* LoadGenerator(DBLink* table=0);
}
}
#endif



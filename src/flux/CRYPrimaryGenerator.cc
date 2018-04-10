//******************************************************************************
// CRYPrimaryGenerator.cc
//
// 1.00 JMV, LLNL, Jan-2007:  First version.
//******************************************************************************
//

#include <iomanip>
#include "CRYPrimaryGenerator.hh"
#include "db/DB.hh"
#include "db/DBLink.hh"
using namespace std;
#include "G4Event.hh"
namespace COSMIC {

//----------------------------------------------------------------------------//
CRYPrimaryGenerator::CRYPrimaryGenerator()
{
  std::cout << "Building CRY Generator" << std::endl;
  fTable = DB::Get()->GetLink("CRY", "config");

  // Setup Defaults
  fGenNeutrons  = true;
  fGenProtons   = true;
  fGenGammas    = true;
  fGenElectrons = true;
  fGenMuons     = true;
  fGenPions     = true;

  fAltitude = 0.0; // Sea Level
  fLatitude = 0.0; // Equator

  fDate = "1-1-2007"; // CRY Default

  fNParticlesMin = -1; // No Truncation
  fNParticlesMax = -1; // No Truncation

  fLateralBoxSize = -1; // This is tricker, define in GetSourceBox().

  fDataDirectory = DB::GetDataPath() + "/cry/";

  // Allow for overrides
  if (fTable->Has("gen_neutrons"))  fGenNeutrons = fTable->GetB("gen_neutrons");
  if (fTable->Has("gen_protons"))   fGenProtons = fTable->GetB("gen_protons");
  if (fTable->Has("gen_gammas"))    fGenGammas = fTable->GetB("gen_gammas");
  if (fTable->Has("gen_electrons")) fGenElectrons = fTable->GetB("gen_electrons");
  if (fTable->Has("gen_muons"))     fGenMuons = fTable->GetB("gen_muons");
  if (fTable->Has("gen_pions"))     fGenPions = fTable->GetB("gen_pions");

  if (fTable->Has("latitude")) fLatitude = fTable->GetD("latitude");
  if (fTable->Has("altitude")) fAltitude = fTable->GetD("altitude");

  if (fTable->Has("date")) fDate = fTable->GetS("date");

  if (fTable->Has("min_particles")) fNParticlesMin = fTable->GetI("min_particles");
  if (fTable->Has("max_particles")) fNParticlesMax = fTable->GetI("max_particles");

  // Lateral box size is defined from the source box
  GetSourceBox();

  // Call update CRY to load tables given the current config
  UpdateCRY();

  // create a vector to store the CRY particle properties
  vect = new std::vector<CRYParticle*>;

  // Create the table containing all particle names
  particleTable = G4ParticleTable::GetParticleTable();

  // define a particle gun
  particleGun = new G4ParticleGun();
}

//----------------------------------------------------------------------------//
CRYPrimaryGenerator::~CRYPrimaryGenerator()
{
}

//----------------------------------------------------------------------------//
void CRYPrimaryGenerator::UpdateCRY()
{
  std::ostringstream cryconfigs;

  // Fill Particle Definitions
  cryconfigs << " returnNeutrons "  << int(fGenNeutrons);
  cryconfigs << " returnProtons "   << int(fGenProtons);
  cryconfigs << " returnGammas "    << int(fGenGammas);
  cryconfigs << " returnElectrons " << int(fGenElectrons);
  cryconfigs << " returnMuons "     << int(fGenMuons);

  // Fill Altitude, Latitude, and Date Definition
  cryconfigs << " altitude " << fAltitude;
  cryconfigs << " latitude " << fLatitude;
  cryconfigs << " date " << fDate;

  // Fill truncation if provided
  // if (fNParticlesMin > 0) {
    cryconfigs << " nParticlesMin " << fNParticlesMin;
  // }
  if (fNParticlesMax > 0) {
    cryconfigs << " nParticlesMax " << fNParticlesMax;
  }

  // Get Lateral Box Size from the current source box
  cryconfigs << " subboxLength " << fLateralBoxSize;

  // Make input setup
  std::string configinputs = cryconfigs.str();
  CRYSetup *setup = new CRYSetup(configinputs, fDataDirectory);

  // Replace Generator
  // if (gen) delete gen;
  gen = new CRYGenerator(setup);

  // set random number generator
  RNGWrapper<CLHEP::HepRandomEngine>::set(CLHEP::HepRandom::getTheEngine(), &CLHEP::HepRandomEngine::flat);
  setup->setRandomFunction(RNGWrapper<CLHEP::HepRandomEngine>::rng);
  InputState = 0;

}


G4Box* CRYPrimaryGenerator::GetSourceBox() {

  // Already has good source_box
  if (fSourceBox) return fSourceBox;

  std::vector<DBLink*> targetlinks = DB::Get()->GetLinkGroup("FLUX");
  for (uint i = 0; i < targetlinks.size(); i++) {
    DBLink* tbl = targetlinks[i];

    // Select tables with target box names
    std::string index = tbl->GetIndexName();
    if (index.compare("source_box") != 0) continue;

    // Get size and provide some checks
    std::vector<double> size = tbl->GetVecD("size");
    if (size[1] != size[0]) {
      std::cout << "CRY box can only be square! Change to : "
                << size[0] << "*" << size[0] << std::endl;
      throw;
    }

    if (size[2] > 0.5) {
      std::cout << "CRY box z-size shouldn't be larger than 50cm,"
                << " and should be above all other geometry!" << std::endl;
      throw;
    }

    // Set our lateral box size for the CRY Engine 
    fLateralBoxSize = size[0];

    // Create the box
    fSourceBox = new G4Box(index, 0.5 * size[0]*m, 0.5 * size[1]*m, 0.5 * size[2]*m);

    // Find box placement
    std::vector<double> pos = tbl->GetVecD("position");
    fSourceBoxPosition = G4ThreeVector(pos[0] * m, pos[1] * m, pos[2] * m);

    break;
  }
  if (fSourceBox) return fSourceBox;

  // Cant find
  std::cout << "Cannot find source box table!" << std::endl;
  throw;
}

std::vector<G4Box*> CRYPrimaryGenerator::GetTargetBoxes() {

  // If presences of target boxes already been set then just return
  if (fCheckTargetBoxes) {
    return fTargetBoxes;
  }

  // If its not set but we have boxes return boxes
  if (fTargetBoxes.size() > 0) {
    fCheckTargetBoxes = true;
    return fTargetBoxes;
  }

  // If none set then make it
  std::vector<DBLink*> targetlinks = DB::Get()->GetLinkGroup("FLUX");
  for (uint i = 0; i < targetlinks.size(); i++) {
    DBLink* tbl = targetlinks[i];

    // Select tables with target box names
    std::string index = tbl->GetIndexName();
    if (index.find("target_box_") == std::string::npos) continue;

    // If it has position and size we can use it
    if (!tbl->Has("position") || !tbl->Has("size")) {
      std::cout << "Failed to find/create target box!" << std::endl;
      throw;
    }

    // Create objects
    std::vector<double> size = tbl->GetVecD("size");
    std::vector<double> pos = tbl->GetVecD("position");

    G4Box* box_sol = new G4Box(index, 0.5 * size[0]*m, 0.5 * size[1]*m, 0.5 * size[2]*m);
    G4ThreeVector box_pos = G4ThreeVector(pos[0] * m, pos[1] * m, pos[2] * m);

    // Save Box
    fTargetBoxes.push_back(box_sol);
    fTargetBoxPositions.push_back(box_pos);
  }

  // Set flag and return
  fCheckTargetBoxes = true;
  return fTargetBoxes;
}

void CRYPrimaryGenerator::GeneratePrimaries(G4Event* anEvent)
{
  if (InputState != 0) {
    G4String* str = new G4String("CRY library was not successfully initialized");
    //G4Exception(*str);
    G4Exception("CRYPrimaryGenerator", "1",
                RunMustBeAborted, *str);
  }

  // Run Initial Setup Incase we are in a seperate thread
  if (!fSourceBox) {
    // G4AutoLock lock(&myMutex);
    GetSourceBox();
    // lock.unlock();
  }

  if (!fCheckTargetBoxes) {
    // G4AutoLock lock(&myMutex);
    GetTargetBoxes();
    // lock.unlock();
  }



  G4String particleName;
  uint stacksize = 0;
  uint throws = 0;

  do {
    throws++;
    vect->clear();
    gen->genEvent(vect);

    // Number of trajectories that intercept at least one targetbox
    stacksize = 0;

    // Loop over all vectors and change their y values to match our box position
    for (unsigned j = 0; j < vect->size(); j++) {

      // Apply offsets according to where the fSourceBox was placed.
      G4ThreeVector boxposthrow = fSourceBox->GetPointOnSurface() + fSourceBoxPosition;

      // Setup new vector position
      G4ThreeVector position  = G4ThreeVector((*vect)[j]->x() * m, (*vect)[j]->y() * m, boxposthrow[2]);
      (*vect)[j]->setPosition(position[0] / m, position[1] / m, position[2] / m);

      // Get Direction for trjacetory pre-selection
      G4ThreeVector direction = G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w());
      bool good_traj = (fTargetBoxes.size() == 0);

      // Make sure trajectory falls inside our target box if we have one.
      for (uint i = 0; i < fTargetBoxes.size(); i++) {
        G4double d = (fTargetBoxes.at(i))->DistanceToIn(
                       position - fTargetBoxPositions.at(i), direction);

        if (d != kInfinity) {
          good_traj = true;
          break;
        }
      }

      // If one good trajectory then event is good.
      if (good_traj) { stacksize++; }
    }

  } while (stacksize < 1 and throws < 1E8);

  // Throw if couldn't create events with good trajectories
  if (throws >= 1E8) {
    std::cout << "Failed to find any good events in 1E8 tries!" << std::endl;
    throw;
  }

  //....debug output
  // G4cout << "\nEvent=" << anEvent->GetEventID() << " "
         // << "CRY generated nparticles=" << vect->size()
         // << G4endl;

  for ( unsigned j = 0; j < vect->size(); j++) {

    // Skip if trajectory was NULL
    if (!(*vect)[j]) continue;

    // // Get Name
    // particleName = CRYUtils::partName((*vect)[j]->id());

    // //....debug output
    // cout << "  "          << particleName << " "
    //      << "charge="      << (*vect)[j]->charge() << " "
    //      << setprecision(4)
    //      << "energy (MeV)=" << (*vect)[j]->ke()*MeV << " "
    //      << "pos (m)"
    //      << G4ThreeVector((*vect)[j]->x(), (*vect)[j]->y(), (*vect)[j]->z())
    //      << " " << "direction cosines "
    //      << G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w())
    //      << " " << endl;

    // Add particles to gun
    particleGun->SetParticleDefinition(particleTable->FindParticle((*vect)[j]->PDGid()));
    particleGun->SetParticleEnergy((*vect)[j]->ke()*MeV);
    particleGun->SetParticlePosition(G4ThreeVector((*vect)[j]->x()*m, (*vect)[j]->y()*m, (*vect)[j]->z()*m));
    particleGun->SetParticleMomentumDirection(G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()));
    particleGun->SetParticleTime((*vect)[j]->t());
    particleGun->GeneratePrimaryVertex(anEvent);

    // Remove this particle
    delete (*vect)[j];
  }
}
}
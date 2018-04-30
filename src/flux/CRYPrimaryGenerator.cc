//******************************************************************************
// CRYPrimaryGenerator.cc
//
// 1.00 JMV, LLNL, Jan-2007:  First version.
//******************************************************************************
//
#include "CRYPrimaryGenerator.hh"
#include "db/DB.hh"

namespace COSMIC {

//----------------------------------------------------------------------------//
CRYPrimaryGenerator::CRYPrimaryGenerator() :
InputState(-1),
fSourceBox(0)
{
  std::cout << "FLX: Building CRY Generator" << std::endl;
  DBTable table = DB::Get()->GetTable("CRY", "config");

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
  if (table.Has("gen_neutrons"))  fGenNeutrons = table.GetB("gen_neutrons");
  if (table.Has("gen_protons"))   fGenProtons = table.GetB("gen_protons");
  if (table.Has("gen_gammas"))    fGenGammas = table.GetB("gen_gammas");
  if (table.Has("gen_electrons")) fGenElectrons = table.GetB("gen_electrons");
  if (table.Has("gen_muons"))     fGenMuons = table.GetB("gen_muons");
  if (table.Has("gen_pions"))     fGenPions = table.GetB("gen_pions");

  if (table.Has("latitude")) fLatitude = table.GetD("latitude");
  if (table.Has("altitude")) fAltitude = table.GetD("altitude");

  if (table.Has("date")) fDate = table.GetS("date");

  if (table.Has("min_particles")) fNParticlesMin = table.GetI("min_particles");
  if (table.Has("max_particles")) fNParticlesMax = table.GetI("max_particles");

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

  // Set exposure and nthrows
  fExposureTime = 0.0;
  fNthrows = 0;

  // Make a new processor
  Analysis::Get()->SetFluxProcessor(new CRYPrimaryFluxProcessor(this));
}

//----------------------------------------------------------------------------//
CRYPrimaryGenerator::~CRYPrimaryGenerator()
{
  delete fSourceBox;
  delete gen;
  vect->clear();
  delete vect;
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
  if (fNParticlesMin > 0) {
  cryconfigs << " nParticlesMin " << fNParticlesMin;
  }
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
  std::cout << "FLX: --> Creating Source box" << std::endl;

  std::vector<DBTable> targetlinks = DB::Get()->GetTableGroup("FLUX");
  for (uint i = 0; i < targetlinks.size(); i++) {
    DBTable tbl = (targetlinks[i]);

    // Select tables with target box names
    std::string index = tbl.GetIndexName();
    if (index.compare("source_box") != 0) continue;

    // Get size and provide some checks
    std::vector<G4double> size = tbl.GetVecG4D("size");
    if (size[1] != size[0]) {
      std::cout << "CRY box can only be square! Change to : "
                << size[0] << "*" << size[0] << std::endl;
      throw;
    }

    if (size[2] / m > 0.5) {
      std::cout << "CRY box z-size shouldn't be larger than 50cm,"
                << " and should be above all other geometry!" << std::endl;
      throw;
    }

    // Set our lateral box size for the CRY Engine
    fLateralBoxSize = size[0] / m;

    // Create the box
    fSourceBox = new G4Box(index, 0.5 * size[0], 0.5 * size[1], 0.5 * size[2]);
    // Find box placement
    std::vector<G4double> pos = tbl.GetVecG4D("position");
    fSourceBoxPosition = G4ThreeVector(pos[0], pos[1], pos[2]);

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
  std::cout << "FLX: --> Creating Target boxes" << std::endl;

  // If none set then make it
  std::vector<DBTable> targetlinks = DB::Get()->GetTableGroup("FLUX");
  for (uint i = 0; i < targetlinks.size(); i++) {
    DBTable tbl = targetlinks[i];

    // Select tables with target box names
    std::string index = tbl.GetIndexName();
    if (index.find("target_box_") == std::string::npos) continue;

    // If it has position and size we can use it
    if (!tbl.Has("position") || !tbl.Has("size")) {
      std::cout << "Failed to find/create target box!" << std::endl;
      throw;
    }

    // Create objects
    std::vector<G4double> size = tbl.GetVecG4D("size");
    std::vector<G4double> pos = tbl.GetVecG4D("position");

    G4Box* box_sol = new G4Box(index, 0.5 * size[0], 0.5 * size[1], 0.5 * size[2]);
    G4ThreeVector box_pos = G4ThreeVector(pos[0], pos[1], pos[2]);

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
    fExposureTime = gen->timeSimulated();
    fNthrows++;

    // std::cout << " Throws : " << fNthrows << " Exposure : " << fExposureTime << std::endl;

    // Number of trajectories that intercept at least one targetbox
    stacksize = 0;
    // Loop over all vectors and change their y values to match our box position
    for (unsigned j = 0; j < vect->size(); j++) {

      // Apply offsets according to where the fSourceBox was placed.
      G4ThreeVector boxposthrow = fSourceBox->GetPointOnSurface() + fSourceBoxPosition;

      // Setup new vector position
      G4ThreeVector position  = G4ThreeVector((*vect)[j]->x() * m, (*vect)[j]->y() * m, boxposthrow[2]);
      (*vect)[j]->setPosition(position[0], position[1], position[2]);

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
      if (good_traj) {
        stacksize++;
      }
    }

    if (stacksize == 0) {
      for (unsigned j = 0; j < vect->size(); j++) {
        delete (*vect)[j];
      }
    }

  } while (stacksize < 1 and throws < 1E8);

  // Throw if couldn't create events with good trajectories
  if (throws >= 1E8) {
    std::cout << "Failed to find any good events in 1E8 tries!" << std::endl;
    throw;
  }

  // std::cout << "GEN Time Simulated : " << fExposureTime << " : " << fNthrows << std::endl;

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
    particleGun->SetParticlePosition(G4ThreeVector((*vect)[j]->x(), (*vect)[j]->y(), (*vect)[j]->z()));
    particleGun->SetParticleMomentumDirection(G4ThreeVector((*vect)[j]->u(), (*vect)[j]->v(), (*vect)[j]->w()));
    particleGun->SetParticleTime((*vect)[j]->t());
    particleGun->GeneratePrimaryVertex(anEvent);

    // Remove this particle
    delete (*vect)[j];
  }
}
//------------------------------------------------------------------


//------------------------------------------------------------------
CRYPrimaryFluxProcessor::CRYPrimaryFluxProcessor(CRYPrimaryGenerator* gen, bool autosave) :
  VFluxProcessor("cry"), fSave(autosave)
{
  fGenerator = gen;
}

bool CRYPrimaryFluxProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = "cry";
  std::cout << "FLX: Registering CRYPrimaryFluxProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");

  return true;
}

bool CRYPrimaryFluxProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Register Trigger State
  fHasInfo = true;
  fTime    = fGenerator->GetTime();

  // Set Ntuple to defaults
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->FillNtupleDColumn(fTimeIndex, -999.);

  if (fHasInfo) {
    // Fill muon vectors
    man->FillNtupleDColumn(fTimeIndex,   fGenerator->GetTime());
    return true;
  } else {
    return false;
  }
  return true;
}

G4double CRYPrimaryFluxProcessor::GetExposureTime() {
  return fGenerator->GetTime();
}
//------------------------------------------------------------------

} // - namespace COSMIC

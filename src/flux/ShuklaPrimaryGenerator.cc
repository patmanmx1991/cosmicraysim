#include "ShuklaPrimaryGenerator.hh"
#include "db/DB.hh"

namespace COSMIC {

ShuklaPrimaryGenerator::ShuklaPrimaryGenerator()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0)
{
    G4AutoLock lock(&myMutex);
    std::cout << "Building Shukla Generator" << std::endl;

    // Setup Table
    fTable = DB::Get()->GetLink("SHUKLA", "config");

    // Setup Defaults + Table Inputs
    fMinEnergy = 0.1;
    fMaxEnergy = 5000.0;

    fPar_I0  = 88.5; // m-2 s-1 sr-1
    fPar_n   = 3.00;
    fPar_E0  = 4.28;
    fPar_eps = 854;
    fPar_rad = 6371.0;
    fPar_dis = 36.61;

    // Check for predef sets
    if (fTable->Has("parameters")) {
        std::string parset = fTable->GetS("parameters");

        if (parset.compare("nottingham") == 0) {
            std::cout << " --> Using Nottingham parset." << std::endl;

        } else if (parset.compare("princealbert")) {
            std::cout << " --> Using Nottingham parset." << std::endl;
            fPar_I0 = 110.0;
            fPar_E0 = 3.6;

        } else if (parset.compare("hamburg") == 0) {
            std::cout << " --> Using Hamburg parset." << std::endl;
            fPar_I0  = 71.0;
            fPar_E0  = 23.78;
            fPar_eps = 2000.0;

        } else if (parset.compare("proton") == 0) {
            std::cout << " --> Using Proton parset." << std::endl;
            fPar_I0 = 8952.0;
            fPar_n = 2.93;
            fPar_E0 = 1.42;
            fPar_eps = 0.0;

        } else if (parset.compare("helium") == 0) {
            std::cout << " --> Using Helium parset." << std::endl;
            fPar_I0 = 5200.0;
            fPar_n = 2.75;
            fPar_E0 = 0.28;
            fPar_eps = 0.0;
        }
    } else {
        std::cout << " --> Using Nottingham parset." << std::endl;
    }

    // Now look for manual overrides
    if (fTable->Has("min_energy")) fMaxEnergy = fTable->GetD("min_energy");
    if (fTable->Has("max_energy")) fMaxEnergy = fTable->GetD("max_energy");

    if (fTable->Has("I0")) fPar_I0 = fTable->GetD("I0");
    if (fTable->Has("n"))  fPar_n  = fTable->GetD("n");
    if (fTable->Has("E0")) fPar_E0 = fTable->GetD("E0");
    if (fTable->Has("epsilon"))  fPar_eps = fTable->GetD("epsilon");
    if (fTable->Has("radius"))   fPar_rad = fTable->GetD("radius");
    if (fTable->Has("distance")) fPar_dis = fTable->GetD("distance");

    // Print Setup
    std::cout << " --> Min Energy : " << fMinEnergy << " GeV" << std::endl;
    std::cout << " --> Max Energy : " << fMaxEnergy << " GeV" << std::endl;
    std::cout << " --> I0         : " << fPar_I0 << " m-2 s-1 sr -1" << std::endl;
    std::cout << " --> n          : " << fPar_n << std::endl;
    std::cout << " --> E0         : " << fPar_E0  << " GeV" << std::endl;
    std::cout << " --> epsilon    : " << fPar_eps << " GeV" << std::endl;
    std::cout << " --> radius     : " << fPar_rad << " km"  << std::endl;
    std::cout << " --> distance   : " << fPar_dis << " km"  << std::endl;
    std::cout << " --> R/d        : " << fPar_rad / fPar_dis << std::endl;

    // Check limits
    if (fMinEnergy < 0.1) {
        std::cout << "min_energy must be >= 0.1 GeV!" << std::endl;
        throw;
    }
    if (fMaxEnergy > 5000.0) {
        std::cout << "max_energy must be <= 5000 GeV" << std::endl;
        throw;
    }

    // // Setup the energy PDF
    std::cout << " --> Setting up PDFs." << std::endl;
    fEnergyPDF = new TF1("energy_pdf",
                         "[3]*((x+[0])**(-[2]))*([1]/([1]+x))", fMinEnergy * GeV, fMaxEnergy * GeV);

    // 0 degrees parameters from fit in Nottingham
    fEnergyPDF->SetParameter(0, fPar_E0 * GeV);
    fEnergyPDF->SetParameter(1, fPar_eps * GeV);
    fEnergyPDF->SetParameter(2, fPar_n);
    fEnergyPDF->SetParameter(3, 1.0);

    // Normalize the PDF
    G4double norm = fEnergyPDF->Integral(fMinEnergy * GeV, fMaxEnergy * GeV);
    fEnergyPDF->SetParameter(3, 1.0 / norm);

    // From dataset fit in Shukla paper
    G4double vertical_flux_rate = fPar_I0;// m-2 s-1 sr-1
    std::cout << " --> VerticalFlux : " << vertical_flux_rate << " m-2 s-1 sr-1" << std::endl;

    // Parameters : [0] = r, x = cos_theta
    fZenithPDF = new TF1("fZenithPDF",
                         "(TMath::Sqrt([0]*[0]*x*x+2*[0]+1) - [0]*x)**(-[1]+1)", 0.0, 1.0);
    fZenithPDF->SetParameter(0, fPar_rad / fPar_dis);
    fZenithPDF->SetParameter(1, fPar_n);

    // For a vertical source, 0 < phi < 2*pi (both directions), 0 < theta < pi/2 (0 < cos_theta < 1)
    fFluxIntegrated = (vertical_flux_rate * CLHEP::twopi
                       * fZenithPDF->Integral(0.0, 1.0)
                       * fEnergyPDF->Integral(fMinEnergy * GeV, fMaxEnergy * GeV));

    // The muon rate
    std::cout << " --> Integrated Flux : " << fFluxIntegrated << " m-2 s-1" << std::endl;
    std::cout << " --> Integrated Flux : " << 60.0 * fFluxIntegrated << " m-2 min-1" << std::endl;

    // Setup Particle Gun
    std::cout << " --> Creating Particle Gun." << std::endl;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

    // Save particle definitions
    fParticleDefs.push_back(particleTable->FindParticle("mu-"));
    fParticleDefs.push_back(particleTable->FindParticle("mu+"));

    // Now setup the particle integrals and source/target boxes
    fEnergyPDF->SetRange(fMinEnergy, fMaxEnergy);
    GetSourceBox();
    GetTargetBoxes();
    std::cout << " --> Complete." << std::endl;
}

ShuklaPrimaryGenerator::~ShuklaPrimaryGenerator()
{
    delete fParticleGun;
}

G4ThreeVector ShuklaPrimaryGenerator::SampleDirection() {
    G4double cos_theta = fZenithPDF->GetRandom();
    G4double phi = CLHEP::twopi * G4UniformRand(); //phi uniform in [-pi, pi];
    G4double sin_theta = std::sqrt(1 - cos_theta * cos_theta);
    G4double x = sin_theta * cos(phi);
    G4double y = sin_theta * sin(phi);
    G4double z = cos_theta;
    return G4ThreeVector(x, y, -z);
}

void ShuklaPrimaryGenerator::SampleParticleType() {
    // From fig 24.5 in PDG Fmu+/Fmu- is 1.3, so Fmu-/(Fmu+Fmu-) = 0.43
    // At a future date, an effect due to energy should be included.
    G4double r = G4UniformRand();
    if (r < 0.43) {
        fParticleGun->SetParticleDefinition(fParticleDefs[0]);
    } else {
        fParticleGun->SetParticleDefinition(fParticleDefs[1]);
    }
}

G4Box* ShuklaPrimaryGenerator::GetSourceBox() {

    // Already has good source_box
    if (fSourceBox) return fSourceBox;

    std::vector<DBLink*> targetlinks = DB::Get()->GetLinkGroup("FLUX");
    for (uint i = 0; i < targetlinks.size(); i++) {
        DBLink* tbl = targetlinks[i];

        // Select tables with target box names
        std::string index = tbl->GetIndexName();
        if (index.compare("source_box") != 0) continue;

        std::vector<double> size = tbl->GetVecD("size");
        std::vector<double> pos = tbl->GetVecD("position");
        fSourceBox = new G4Box(index, 0.5 * size[0]*m, 0.5 * size[1]*m, 0.5 * size[2]*m);
        fSourceBoxPosition = G4ThreeVector(pos[0] * m, pos[1] * m, pos[2] * m);

        break;
    }
    if (fSourceBox) return fSourceBox;

    // Cant find
    std::cout << "Cannot find source box table!" << std::endl;
    throw;
}

std::vector<G4Box*> ShuklaPrimaryGenerator::GetTargetBoxes() {

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

std::vector<G4ThreeVector> ShuklaPrimaryGenerator::GetTargetBoxPositions() {
    // If matching sizes its probs okay to return positions
    if (fTargetBoxes.size() == fTargetBoxPositions.size()) return fTargetBoxPositions;
    std::cout << "TargetBox Positions incorrect" << std::endl;
    throw;
}

void ShuklaPrimaryGenerator::GeneratePrimaries(G4Event* anEvent) {

    // Run Initial Setup Incase we are in a seperate thread
    if (!fSourceBox) {
        G4AutoLock lock(&myMutex);
        GetSourceBox();
        lock.unlock();
    }

    if (!fCheckTargetBoxes) {
        G4AutoLock lock(&myMutex);
        GetTargetBoxes();
        lock.unlock();
    }

    // Create a mutex lock for the ROOT sampling functions (not thread-safe)
    // G4AutoLock lock(&myMutex);

    // By setting a fMinEnergy it is possible to remove a portion
    // of the muon energy spectra which you're sure will not contribute
    // to the detector counts
    // i.e. for a geometry involving a thick layer of rock, you could ignore
    // the energies that will definitely range out before the detector.
    // These muons with energy<fMinEnergy will be generated but rejected here.
    fEnergyPDF->SetRange(fMinEnergy, fMaxEnergy); // E in MeV
    G4double global_time = 0;

    // Sample the energy and particle type
    G4double E = 0.0;
    do {
        E = fEnergyPDF->GetRandom();
    } while (E < fMinEnergy * GeV);
    SampleParticleType();

    // Start of the rejection sampling of muon tracks
    bool good_event = false;
    uint throws = 0;
    G4ThreeVector direction = G4ThreeVector();
    G4ThreeVector position = G4ThreeVector();

    do {
        throws++;
        // Sample point and direction
        direction = SampleDirection();
        position  = fSourceBox->GetPointOnSurface() + fSourceBoxPosition;

        // If no target boxes defined all events are good
        if (fTargetBoxes.size() == 0) break;

        // If target boxes defined only save trajectories that hit at least one
        for (uint i = 0; i < fTargetBoxes.size(); i++) {

            G4double d = (fTargetBoxes.at(i))->DistanceToIn(
                             position - fTargetBoxPositions.at(i), direction);

            if (d != kInfinity) {
                good_event = true;
                break;
            }
        }
    } while (!good_event and throws < 1E8);

    if (throws >= 1E8) {
        std::cout << "Failed to find any good events in 1E6 tries!" << std::endl;
        throw;
    }

    // Generate Primary
    // std::cout << "EventID " << anEvent->GetEventID() << G4endl;
    // std::cout << " --> Position " << position << G4endl;
    // std::cout << " --> Direction " << direction << G4endl;
    // std::cout << " --> Global Time " << global_time << G4endl;

    fParticleGun->SetParticleEnergy(E);
    fParticleGun->SetParticleTime(global_time);
    fParticleGun->SetParticleMomentumDirection(direction);
    fParticleGun->SetParticlePosition(position);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    return;
}

} // - namespace COSMIC
#include "ShuklaPrimaryGenerator.hh"
#include "db/DB.hh"

namespace COSMIC {

//---------------------------------------------------------------------------------
ShuklaPrimaryGenerator::ShuklaPrimaryGenerator()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(0),
      fNThrows(0),
      fMuonTime(0)
{
    G4AutoLock lock(&myMutex);
    std::cout << "FLX: Building Shukla Generator" << std::endl;

    // Setup Table
    DBTable table = DB::Get()->GetTable("SHUKLA", "config");

    // Get the particle table
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    // Save particle definitions
    fParticleDefs.push_back(particleTable->FindParticle("mu-"));
    fParticleDefs.push_back(particleTable->FindParticle("mu+"));
    fParticleDefs.push_back(particleTable->FindParticle("proton"));
    fParticleDefs.push_back(particleTable->FindParticle("alpha"));


    // Setup Defaults + Table Inputs
    fMinEnergy = 0.1;
    fMaxEnergy = 10000.0;

    fPar_I0  = 88.5; // m-2 s-1 sr-1
    fPar_n   = 3.00;
    fPar_E0  = 4.28;
    fPar_eps = 854;
    fPar_rad = 6371.0;
    fPar_dis = 36.61;
    fMuonPDG = 13;//  Initialize this as a muon first (samples +/- later on)

    // Check for predef sets
    if (table.Has("parameters")) {
        std::string parset = table.GetS("parameters");

        if (parset.compare("nottingham") == 0) {
            std::cout << "FLX: --> Using Nottingham parset." << std::endl;

        } else if (parset.compare("princealbert")==0) {
            std::cout << "FLX: --> Using Nottingham-PA parset." << std::endl;
            fPar_I0 = 110.0;
            fPar_E0 = 3.6;

        } else if (parset.compare("hamburg") == 0) {
            std::cout << "FLX: --> Using Hamburg parset." << std::endl;
            fPar_I0  = 71.0;
            fPar_E0  = 23.78;
            fPar_eps = 2000.0;

        } else if (parset.compare("proton") == 0) {
            std::cout << "FLX: --> Using Proton parset." << std::endl;
            fPar_I0 = 8952.0;
            fPar_n = 2.93;
            fPar_E0 = 1.42;
            fPar_eps = 1e12;// 1/epsilon = 0

            // Set particle as a proton
            fMuonPDG = 2212;

        } else if (parset.compare("helium") == 0) {
            std::cout << "FLX: --> Using Helium parset." << std::endl;
            fPar_I0 = 5200.0;
            fPar_n = 2.75;
            fPar_E0 = 0.28;
            fPar_eps = 1e12;// 1/epsilon = 0
            // Set particle as an alpha particle
            fMuonPDG = 47;
        }
    } else {
        std::cout << "FLX: --> Using default Nottingham parset." << std::endl;
    }

    // Now look for manual overrides
    if (table.Has("min_energy")) fMinEnergy = table.GetD("min_energy");
    if (table.Has("max_energy")) fMaxEnergy = table.GetD("max_energy");

    if (table.Has("I0"))       fPar_I0  = table.GetD("I0");
    if (table.Has("n"))        fPar_n   = table.GetD("n");
    if (table.Has("E0"))       fPar_E0  = table.GetD("E0");
    if (table.Has("epsilon"))  fPar_eps = table.GetD("epsilon");
    if (table.Has("radius"))   fPar_rad = table.GetD("radius");
    if (table.Has("distance")) fPar_dis = table.GetD("distance");

    // Print Setup
    std::cout << "FLX: --> Min Energy : " << fMinEnergy << " GeV" << std::endl;
    std::cout << "FLX: --> Max Energy : " << fMaxEnergy << " GeV" << std::endl;
    std::cout << "FLX: --> I0         : " << fPar_I0 << " m-2 s-1 sr -1" << std::endl;
    std::cout << "FLX: --> n          : " << fPar_n << std::endl;
    std::cout << "FLX: --> E0         : " << fPar_E0 << " GeV" << std::endl;
    std::cout << "FLX: --> epsilon    : " << fPar_eps << " GeV" << std::endl;
    std::cout << "FLX: --> radius     : " << fPar_rad << " km"  << std::endl;
    std::cout << "FLX: --> distance   : " << fPar_dis << " km"  << std::endl;
    std::cout << "FLX: --> R/d        : " << fPar_rad / fPar_dis << std::endl;

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
    std::cout << "FLX: --> Setting up PDFs." << std::endl;
    fEnergyPDF = new TF1("energy_pdf",
                         "[3]*((x+[0])**(-[2]))*([1]/([1]+x))", fMinEnergy * GeV, fMaxEnergy * GeV);

    // 0 degrees parameters from fit in Nottingham
    fEnergyPDF->SetParameter(0, fPar_E0 * GeV);
    fEnergyPDF->SetParameter(1, fPar_eps * GeV);
    fEnergyPDF->SetParameter(2, fPar_n);
    fEnergyPDF->SetParameter(3, 1.0);

    // Normalize the PDF
    G4double norm = fEnergyPDF->Integral(0.1 * GeV, 5000.0 * GeV);
    fEnergyPDF->SetParameter(3, 1.0 / norm);

    // Get the speed up factor
    fSpeedUp = fEnergyPDF->Integral(fMinEnergy * GeV, fMaxEnergy * GeV);
    std::cout << "FLX: --> fSpeedUp        : " << fSpeedUp << std::endl;

    // From dataset fit in Shukla paper
    G4double vertical_flux_rate = fPar_I0;// m-2 s-1 sr-1
    std::cout << "FLX: --> VerticalFlux : " << vertical_flux_rate << " m-2 s-1 sr-1" << std::endl;

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
    std::cout << "FLX: --> Integrated Flux : " << fFluxIntegrated << " m-2 s-1" << std::endl;
    std::cout << "FLX: --> Integrated Flux : " << 60.0 * fFluxIntegrated*cm*cm / m / m  << " cm-2 min-1" << std::endl;

    // Setup Particle Gun
    std::cout << "FLX: --> Creating Particle Gun." << std::endl;
    G4int nofParticles = 1;
    fParticleGun  = new G4ParticleGun(nofParticles);


    // Now setup the particle integrals and source/target boxes
    fEnergyPDF->SetRange(fMinEnergy, fMaxEnergy);
    fSourceBox = false;
    GetSourceBox();
    GetTargetBoxes();
    fEnergyPDF->SetRange(fMinEnergy, fMaxEnergy); // E in MeV

    std::cout << "FLX: --> Complete." << std::endl;

    // Make a new processor
    Analysis::Get()->SetFluxProcessor(new ShuklaPrimaryFluxProcessor(this));
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

  if(fabs(fMuonPDG)==13) {
    // From fig 24.5 in PDG Fmu+/Fmu- is 1.3, so Fmu-/(Fmu+Fmu-) = 0.43
    // At a future date, an effect due to energy should be included.
    G4double r = G4UniformRand();
    if (r < 0.43) {
        fParticleGun->SetParticleDefinition(fParticleDefs[0]);
        fMuonPDG = 13;
    } else {
        fParticleGun->SetParticleDefinition(fParticleDefs[1]);
        fMuonPDG = -13;
    }
  } else {
    if(fMuonPDG==2212)    fParticleGun->SetParticleDefinition(fParticleDefs[2]);
    if(fMuonPDG==47)    fParticleGun->SetParticleDefinition(fParticleDefs[3]);
  }



}

void ShuklaPrimaryGenerator::GetSourceBox() {

    // Already has good source_box
    if (fSourceBox) return;
    std::cout << "FLX: --> Creating Source box" << std::endl;

    std::vector<DBTable> targetlinks = DB::Get()->GetTableGroup("FLUX");
    for (uint i = 0; i < targetlinks.size(); i++) {
        DBTable tbl = targetlinks[i];

        // Select tables with target box names
        std::string index = tbl.GetIndexName();
        if (index.compare("source_box") != 0) continue;

        std::vector<G4double> size = tbl.GetVecG4D("size");
        std::vector<G4double> pos  = tbl.GetVecG4D("position");
        fSourceBoxWidth    = G4ThreeVector(0.5 * size[0], 0.5 * size[1], 0.0);
        fSourceBoxPosition = G4ThreeVector(pos[0], pos[1], pos[2]);

        if (tbl.Has("require_n")) {
          fSourceBoxRequireN  = tbl.GetI("require_n");
        } else{
          fSourceBoxRequireN  = 1;
        }

        fArea = size[0] * size[1] / m / m;
        break;
    }
    fSourceBox = true;

    if (fSourceBox) return;

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
        std::vector<G4double> pos  = tbl.GetVecG4D("position");

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

    // G4double global_time = 0;//< Not needed as we have fMuonTime?

    // Sample the energy and particle type
    G4double E = 0.0;
    E = fEnergyPDF->GetRandom();

    // Only want to sample the particle if we want muons
    SampleParticleType();

    // Start of the rejection sampling of muon tracks
    bool good_event = false;
    uint throws = 0;
    G4ThreeVector direction = G4ThreeVector();
    G4ThreeVector position = G4ThreeVector();

    G4int num_target_boxes_hit=0;
    // The muon rate
    //  - fArea is in mm (internal G4 units) so need to convert to m
    G4double adjusted_rate = fFluxIntegrated*fArea/fSpeedUp;//< Adjust this rate if we are only sampling a smaller portion of the energy-angle PDF
    // G4cout << "Adj. Rate : " << adjusted_rate << G4endl;

    do {
        throws++;
        // Sample point and direction
        direction = SampleDirection();

        position[0] = fSourceBoxPosition[0] + fSourceBoxWidth[0] * (-1.0 + 2.0 * G4UniformRand()) ;
        position[1] = fSourceBoxPosition[1] + fSourceBoxWidth[1] * (-1.0 + 2.0 * G4UniformRand()) ;
        position[2] = fSourceBoxPosition[2] + fSourceBoxWidth[2] ;

        // Keep track of global throws for integral
        fNThrows++;

        // If no target boxes defined all events are good
        if (fTargetBoxes.empty()) break;

            // If target boxes defined only save trajectories that hit at least one
            for (uint i = 0; i < fTargetBoxes.size(); i++) {

            G4double d = (fTargetBoxes.at(i))->DistanceToIn(
                             position - fTargetBoxPositions.at(i), direction);


            if (d != kInfinity){
              // Increment the counter for the number of target boxes hit
              num_target_boxes_hit++;

              // Check whether we have the required number of hits
              if (num_target_boxes_hit>=fSourceBoxRequireN ) {
                good_event = true;
                break;
              }
            }

            // Regardless of whether the event is accepted increment the time
            fMuonTime -= std::log(1 - G4UniformRand())*(1.0/adjusted_rate);


        }// End for



        if (throws >= 1E8) {
            std::cout << "Failed to find any good events in 1E6 tries!" << std::endl;
            throw;
        }

    } while (!good_event and throws < 1E8);

    // Get exposure time
    // std::cout << "Exposure Time = " << fNThrows << "," << fArea << " : " << fNThrows / fFluxIntegrated << " m^{2} s" << std::endl;
    // std::cout << "Exposure Time : " << fNThrows / fFluxIntegrated / fArea << " s" << std::endl;

    // Generate Primary
    // std::cout << "EventID " << anEvent->GetEventID() << G4endl;
    // std::cout << " --> Position " << position << G4endl;
    // std::cout << " --> Direction " << direction << G4endl;
    // std::cout << " --> Energy " << E*1000 << G4endl;
    // std::cout << " --> Global Time " << fMuonTime << G4endl;


    /// This is incorrect. Need to incremenent the muon exposure time each throw using random stuff from Chris's code.
    // fMuonTime = fNThrows / fFluxIntegrated / fArea;
    fMuonDir = direction;
    fMuonPos = position;
    fMuonEnergy = E*1000.0;

    fParticleGun->SetParticleEnergy(fMuonEnergy);
    fParticleGun->SetParticleTime(fMuonTime);
    fParticleGun->SetParticleMomentumDirection(fMuonDir);
    fParticleGun->SetParticlePosition(fMuonPos);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    return;
}
//---------------------------------------------------------------------------------


//------------------------------------------------------------------
ShuklaPrimaryFluxProcessor::ShuklaPrimaryFluxProcessor(ShuklaPrimaryGenerator* gen, bool autosave) :
    VFluxProcessor("shukla"), fSave(autosave)
{
    fGenerator = gen;
}

bool ShuklaPrimaryFluxProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    std::string tableindex = "shukla";
    std::cout << "FLX: Registering ShuklaPrimaryFluxProcessor NTuples " << tableindex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill index energy
    fMuonTimeIndex   = man ->CreateNtupleDColumn(tableindex + "_t");
    fMuonEnergyIndex = man ->CreateNtupleDColumn(tableindex + "_E");
    fMuonDirXIndex   = man ->CreateNtupleDColumn(tableindex + "_dx");
    fMuonDirYIndex   = man ->CreateNtupleDColumn(tableindex + "_dy");
    fMuonDirZIndex   = man ->CreateNtupleDColumn(tableindex + "_dz");
    fMuonPosXIndex   = man ->CreateNtupleDColumn(tableindex + "_x");
    fMuonPosYIndex   = man ->CreateNtupleDColumn(tableindex + "_y");
    fMuonPosZIndex   = man ->CreateNtupleDColumn(tableindex + "_z");
    fMuonPDGIndex    = man ->CreateNtupleIColumn(tableindex + "_pdg");

    return true;
}

bool ShuklaPrimaryFluxProcessor::ProcessEvent(const G4Event* /*event*/) {

    // Register Trigger State
    fHasInfo = true;
    fTime    = fGenerator->GetMuonTime();
    fEnergy  = fGenerator->GetMuonEnergy();

    // Set Ntuple to defaults

    if (fHasInfo) {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fMuonTimeIndex,   fGenerator->GetMuonTime());
        man->FillNtupleDColumn(fMuonEnergyIndex, fGenerator->GetMuonEnergy());
        man->FillNtupleDColumn(fMuonDirXIndex,   fGenerator->GetMuonDir().x() / MeV);
        man->FillNtupleDColumn(fMuonDirYIndex,   fGenerator->GetMuonDir().y() / MeV);
        man->FillNtupleDColumn(fMuonDirZIndex,   fGenerator->GetMuonDir().z() / MeV);
        man->FillNtupleDColumn(fMuonPosXIndex,   fGenerator->GetMuonPos().x() / m);
        man->FillNtupleDColumn(fMuonPosYIndex,   fGenerator->GetMuonPos().y() / m);
        man->FillNtupleDColumn(fMuonPosZIndex,   fGenerator->GetMuonPos().z() / m);
        man->FillNtupleIColumn(fMuonPDGIndex ,   fGenerator->GetMuonPDG());
        return true;
    } else {
        G4AnalysisManager* man = G4AnalysisManager::Instance();
        man->FillNtupleDColumn(fMuonTimeIndex, -999.);
        man->FillNtupleDColumn(fMuonEnergyIndex, -999.);
        man->FillNtupleDColumn(fMuonDirXIndex, -999.);
        man->FillNtupleDColumn(fMuonDirYIndex, -999.);
        man->FillNtupleDColumn(fMuonDirZIndex, -999.);
        man->FillNtupleDColumn(fMuonPosXIndex, -999.);
        man->FillNtupleDColumn(fMuonPosYIndex, -999.);
        man->FillNtupleDColumn(fMuonPosZIndex, -999.);
        man->FillNtupleIColumn(fMuonPDGIndex,  -999 );
        return false;
    }
    return true;
}

G4double ShuklaPrimaryFluxProcessor::GetExposureTime() {
    return fGenerator->GetMuonTime();
}
//---------------------------------------------------------------------------------

} // - namespace COSMIC

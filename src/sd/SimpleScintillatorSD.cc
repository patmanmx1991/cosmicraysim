#include "SimpleScintillatorSD.hh"
#include "analysis/Analysis.hh"
#include "db/DBLink.hh"
#include "ScintillatorHit.hh"

namespace COSMIC {

//------------------------------------------------------------------
SimpleScintillatorSD::SimpleScintillatorSD(DBLink* tbl):
    VDetector(tbl->GetIndexName(), "scintillator")
{
    std::cout << "Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // By default also include the auto processor
    if (!tbl->Has("processor") or tbl->GetI("processor") > 0) {
        Analysis::Get()->RegisterProcessor(new SimpleScintillatorProcessor(this));
    }

    collectionName.push_back(GetID());
}

SimpleScintillatorSD::SimpleScintillatorSD(std::string name, std::string id,
        bool autoprocess, bool autosave):
    VDetector(name, id)
{
    std::cout << "Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // By default also include the auto processor
    if (autoprocess) {
        Analysis::Get()->RegisterProcessor(new SimpleScintillatorProcessor(this, autosave));
    }

    collectionName.push_back(GetID());
}

void SimpleScintillatorSD::Initialize(G4HCofThisEvent* hce)
{
    std::cout << "Initialising Hit Collection" << std::endl;
    fHitsCollection = new ScintillatorHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4bool SimpleScintillatorSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/) {

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    G4Track* track = step->GetTrack();
    int pdg = track->GetParticleDefinition()->GetPDGEncoding();

    // Get the step inside the detector
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4TouchableHistory* touchable = (G4TouchableHistory*)(preStepPoint->GetTouchable());
    G4int copyNo = touchable->GetVolume()->GetCopyNo();

    // Get the hitTime
    G4double hitTime = preStepPoint->GetGlobalTime();

    ScintillatorHit* hit = new ScintillatorHit();
    hit->SetParticleType(pdg);
    hit->SetEdep(edep);
    hit->SetTime(hitTime);
    hit->SetPos(preStepPoint->GetPosition());
    hit->SetAngles(track->GetMomentumDirection());
    // std::cout << "Recording scintillator hit" << std::endl;
    fHitsCollection->insert(hit);

    return true;
}


//------------------------------------------------------------------
SimpleScintillatorProcessor::SimpleScintillatorProcessor(SimpleScintillatorSD* trkr, bool autosave) :
    VProcessor(trkr->GetID()), fSave(autosave)
{
    fDetector = trkr;
    fHCID = trkr->GetHCID();
}

bool SimpleScintillatorProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    std::string tableindex = GetID();
    std::cout << "Registering ScintillatorSD NTuples " << tableindex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill index energy
    fEdepIndex = man ->CreateNtupleDColumn(tableindex + "_E");
    fTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
    fPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
    fPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
    fPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");
    fThXZIndex = man ->CreateNtupleDColumn(tableindex + "_thXZ");
    fThYZIndex = man ->CreateNtupleDColumn(tableindex + "_thYZ");

    Reset();
    return true;
}

bool SimpleScintillatorProcessor::ProcessEvent(const G4Event* event) {

    // Set default values
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fEdepIndex, -999.);
    man->FillNtupleDColumn(fTimeIndex, -999.);
    man->FillNtupleDColumn(fPosXIndex, -999.);
    man->FillNtupleDColumn(fPosYIndex, -999.);
    man->FillNtupleDColumn(fPosZIndex, -999.);
    man->FillNtupleDColumn(fThXZIndex, -999.);
    man->FillNtupleDColumn(fThYZIndex, -999.);

    // Average over hits
    ScintillatorHitsCollection* hc = static_cast<ScintillatorHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));
    int nhits = (int)  (hc)->GetSize();
    if (nhits < 1) {
        return false;
    }
    // std::cout << "Got Some Hits" << std::endl;
    G4double T = (event->GetPrimaryVertex())->GetT0();

    for (int ihit = 0; ihit < nhits; ihit++) {

        fEdep += ( *(hc) )[ihit]->GetEdep() / MeV;
        fTime += (( ( *(hc) )[ihit]->GetTime() / ns) - T / ns);

        fPosX += ( *(hc) )[ihit]->GetPos()[0] / m;
        fPosY += ( *(hc) )[ihit]->GetPos()[1] / m;
        fPosZ += ( *(hc) )[ihit]->GetPos()[2] / m;

        fThetaXZ += ( *(hc) )[ihit]->GetThetaXZ();
        fThetaYZ += ( *(hc) )[ihit]->GetThetaYZ();
    }

    // Now average
    fTime /= nhits + 0.;

    fPosX /= nhits + 0.;
    fPosY /= nhits + 0.;
    fPosZ /= nhits + 0.;

    fThetaXZ /= nhits + 0.;
    fThetaYZ /= nhits + 0.;

    // Register Trigger State
    fHasInfo = fEdep > 0.0;
    fEnergy  = fEdep;

    // If Triggered then fill
    if (fHasInfo) {

        // Fill muon vectors
        man->FillNtupleDColumn(fEdepIndex, fEdep);
        man->FillNtupleDColumn(fTimeIndex, fTime);
        man->FillNtupleDColumn(fPosXIndex, fPosX);
        man->FillNtupleDColumn(fPosYIndex, fPosY);
        man->FillNtupleDColumn(fPosZIndex, fPosZ);
        man->FillNtupleDColumn(fThXZIndex, fThetaXZ);
        man->FillNtupleDColumn(fThYZIndex, fThetaYZ);

        return true;
    } else {
        return false;
    }
}

void SimpleScintillatorProcessor::Reset() {
    VProcessor::Reset();
    fTime = 0.0;
    fEdep = 0.0;
    fPosX = 0.0;
    fPosY = 0.0;
    fPosZ = 0.0;
    fThetaXZ = 0.0;
    fThetaYZ = 0.0;
}


} // - namespace COSMIC
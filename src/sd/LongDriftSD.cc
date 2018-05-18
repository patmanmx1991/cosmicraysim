#include "LongDriftSD.hh"
#include "analysis/Analysis.hh"
#include "db/DBTable.hh"
#include "DriftHit.hh"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4Square.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Box.hh"
namespace COSMIC {

//------------------------------------------------------------------
LongDriftSD::LongDriftSD(DBTable tbl):
    VDetector(tbl.GetIndexName(), "longdrift")
{
    std::cout << "DET: Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    fEfficiency = tbl.Has("efficiency") ? tbl.GetD("efficiency") : 1.0;

    fResolutionX = tbl.Has("resolution_x") ? tbl.GetG4D("resolution_x") : 0.1 * mm;
    fResolutionY = tbl.Has("resolution_y") ? tbl.GetG4D("resolution_y") : 0.1 * mm;
    fResolutionZ = tbl.Has("resolution_z") ? tbl.GetG4D("resolution_z") : 0.1 * mm;

    fRestrictX = tbl.Has("restrict_x") ? tbl.GetB("restrict_x") : false;
    fRestrictY = tbl.Has("restrict_y") ? tbl.GetB("restrict_y") : false;
    fRestrictZ = tbl.Has("restrict_z") ? tbl.GetB("restrict_z") : false;

    fDetectorSizeX = 0.0;
    fDetectorSizeY = 0.0;
    fDetectorSizeZ = 0.0;
    fHCID = -1;

    fWirePositionX = tbl.Has("wire_x") ? tbl.GetG4D("wire_x") : -999.9;
    fWirePositionY = tbl.Has("wire_y") ? tbl.GetG4D("wire_y") : -999.9;
    fWirePositionZ = tbl.Has("wire_z") ? tbl.GetG4D("wire_z") : -999.9;

    collectionName.push_back(GetID());
    collectionName.push_back(GetID() + "_ghost");

    // By default also include the auto processor
    if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
        Analysis::Get()->RegisterProcessor(new LongDriftProcessor(this));
    }

}

LongDriftSD::LongDriftSD(std::string name, std::string id,
                         bool autoprocess, bool autosave):
    VDetector(name, id)
{
    std::cout << "DET: Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // By default also include the auto processor
    if (autoprocess) {
        Analysis::Get()->RegisterProcessor(new LongDriftProcessor(this, autosave));
    }

    fResolutionX = 0.1 * mm;
    fResolutionY = 0.1 * mm;
    fResolutionZ = 0.1 * mm;

    fRestrictX = true;
    fRestrictY = true;
    fRestrictZ = false;

    fDetectorSizeX = 0.0;
    fDetectorSizeY = 0.0;
    fDetectorSizeZ = 0.0;
    fHCID = -1;

    fWirePositionX = -999.9;
    fWirePositionY = -999.9;
    fWirePositionZ = -999.9;

    collectionName.push_back(GetID());
}

void LongDriftSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new DriftChamberHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4int LongDriftSD::GetHCID() {
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(GetID());
    return fHCID;
}

G4int LongDriftSD::GetGhostHCID() {
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(GetID());
    return fGhostHCID;
}

void LongDriftSD::ResetState() {
    VDetector::ResetState();
    fEfficiencyThrow = G4UniformRand();
}

G4bool LongDriftSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/)
{

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
    if (charge == 0.) return false;

    if (fEfficiency < fEfficiencyThrow) return false;

    G4StepPoint* preStepPoint = step->GetPreStepPoint();

    G4TouchableHistory* touchable
        = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
    G4VPhysicalVolume* motherPhysical = touchable->GetVolume(1); // mother
    G4int copyNo = motherPhysical->GetCopyNo();

    // Get Translations between local and world geometry
    G4AffineTransform trans_worldtolocal = touchable->GetHistory()->GetTopTransform();

    // Get Local and world positions
    G4ThreeVector worldPos = preStepPoint->GetPosition();
    G4ThreeVector localPos = trans_worldtolocal.TransformPoint(worldPos);

    // Apply local smearing
    localPos[0] = G4RandGauss::shoot(localPos[0], fResolutionX);
    localPos[1] = G4RandGauss::shoot(localPos[1], fResolutionY);
    localPos[2] = G4RandGauss::shoot(localPos[2], fResolutionZ);

    // Depending on the detector, we must restrict location in the local geometry
    G4ThreeVector localPosP = localPos;
    G4ThreeVector localPosE;

    // Set Resolution
    localPosE[0] = fResolutionX;
    localPosE[1] = fResolutionY;
    localPosE[2] = fResolutionZ;

    // Restrict to DOF
    if (fRestrictX) {
        localPosP[0] = 0.0 * mm;
        localPosE[0] = fDetectorSizeX;
    }
    if (fRestrictY) {
        localPosP[1] = 0.0 * mm;
        localPosE[1] = fDetectorSizeY;
    }
    if (fRestrictZ) {
        localPosP[2] = 0.0 * mm;
        localPosE[2] = fDetectorSizeZ;
    }

    // World volume seems to be the local point
    G4AffineTransform trans_localtoworld = trans_worldtolocal.Invert();

    G4ThreeVector worldPosP = trans_localtoworld.TransformPoint(localPosP);
    G4ThreeVector emptyE = trans_localtoworld.TransformPoint(G4ThreeVector());
    G4ThreeVector worldPosE = trans_localtoworld.TransformPoint(localPosE);
    worldPosE -= emptyE;

    // Print out hits before and after to check they make sense
    // std::cout << "World Before : " << worldPos[0] / m  << " " << worldPos[1] / m  << " " << worldPos[2] / m << std::endl;
    // std::cout << "Local Before : " << localPos[0] / m  << " " << localPos[1] / m  << " " << localPos[2] / m << std::endl;
    // std::cout << "World After  : " << worldPosP[0] / m << " " << worldPosP[1] / m << " " << worldPosP[2] / m << std::endl;
    // std::cout << "World Error  : " << worldPosE[0] / m << " " << worldPosE[1] / m << " " << worldPosE[2] / m << std::endl;
    // std::cout << "Local After  : " << localPosP[0] / m << " " << localPosP[1] / m << " " << localPosP[2] / m << std::endl;
    // std::cout << "Local Error  : " << localPosE[0] / m << " " << localPosE[1] / m << " " << localPosE[2] / m << std::endl;

    DriftChamberHit* hit = new DriftChamberHit(copyNo);
    hit->SetWorldPos(worldPosP);
    hit->SetWorldPosErr(worldPosE);
    hit->SetLocalPos(localPosP);
    hit->SetLocalPosErr(localPosE);
    hit->SetTime(preStepPoint->GetGlobalTime());
    fHitsCollection->insert(hit);

    /// Drift chambers only get a distance from the wire.
    /// If a wire position in X/Y/Z is given, then make sure we place another one the other side.
    if (fWirePositionX != -999.9 ||
            fWirePositionY != -999.9 ||
            fWirePositionZ != -999.9) {

        G4double updateX = fWirePositionX  - (localPosP[0] - fWirePositionX);
        if (fWirePositionX != -999.9 && fabs(updateX) < fDetectorSizeX) {
            localPosP[0] = updateX;
        }
        G4double updateY = fWirePositionY  - (localPosP[1] - fWirePositionY);
        if (fWirePositionY != -999.9 && fabs(updateY) < fDetectorSizeY) {
            localPosP[1] = updateY;
        }
        G4double updateZ = fWirePositionZ  - (localPosP[0] - fWirePositionZ);
        if (fWirePositionZ != -999.9 && fabs(updateZ) < fDetectorSizeZ) {
            localPosP[2] = updateZ;
        }

        worldPosP = trans_localtoworld.TransformPoint(localPosP);
        hit = new DriftChamberHit(copyNo);
        hit->SetWorldPos(worldPosP);
        hit->SetWorldPosErr(worldPosE);
        hit->SetLocalPos(localPosP);
        hit->SetLocalPosErr(localPosE);
        hit->SetTime(preStepPoint->GetGlobalTime());
        hit->SetGhost(true);
        fHitsCollection->insert(hit);
    }


    return true;
}

void LongDriftSD::DrawAll() {

    std::cout << "DET: Drawing all in Sensitive Detector" << std::endl;
    int nhits = (int) fHitsCollection->GetSize();
    for (int ihit = 0; ihit < nhits; ihit++) {
        ( *(fHitsCollection) )[ihit]->Draw();
    }
    // nhits = (int) fGhostCollection->GetSize();
    // for (int ihit = 0; ihit < nhits; ihit++) {
    //     ( *(fGhostCollection) )[ihit]->Draw();
    // }
}

void LongDriftSD::SetLogicalVolume(G4LogicalVolume* logic, G4VPhysicalVolume* physical) {
    logic->SetSensitiveDetector(static_cast<G4VSensitiveDetector*>(this));

    // Keep track of detectors volume and placement
    fLogical = logic;
    fPhysical = physical;

    // Get the box size
    G4Box* box = static_cast<G4Box*>(fLogical->GetSolid());
    fDetectorSizeX = box->GetXHalfLength();
    fDetectorSizeY = box->GetYHalfLength();
    fDetectorSizeZ = box->GetZHalfLength();
}


//------------------------------------------------------------------
LongDriftProcessor::LongDriftProcessor(LongDriftSD* trkr, bool autosave) :
    VProcessor(trkr->GetID()), fSave(autosave)
{
    fDetector = trkr;
    fHCID = trkr->GetHCID();
}

bool LongDriftProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    if (fSave) {
        std::string tableindex = GetID();
        std::cout << "Registering LongDriftProcessor NTuples " << tableindex << std::endl;

        G4AnalysisManager* man = G4AnalysisManager::Instance();

        // Fill index energy
        fEdepIndex = man ->CreateNtupleDColumn(tableindex + "_E");
        fTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
        fPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
        fPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
        fPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");
        fErrXIndex = man ->CreateNtupleDColumn(tableindex + "_ex");
        fErrYIndex = man ->CreateNtupleDColumn(tableindex + "_ey");
        fErrZIndex = man ->CreateNtupleDColumn(tableindex + "_ez");
    }
    Reset();
    return true;
}

bool LongDriftProcessor::ProcessEvent(const G4Event* event) {


    // Average over hits
    fHCID = fDetector->GetHCID();

    DriftChamberHitsCollection* hc =
        static_cast<DriftChamberHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));
    int nhits = (int)  (hc)->GetSize();

    if (nhits < 1) {
        return false;
    }
    G4double T = (event->GetPrimaryVertex())->GetT0();
    int ngoodhits = 0;
    int nghosthits = 0;
    if (nhits > 0) {
        for (int ihit = 0; ihit < nhits; ihit++) {

            fTime += (( ( *(hc) )[ihit]->GetTime() / ns) - T / ns);

            G4ThreeVector pos = ( *(hc) )[ihit]->GetPos();
            G4ThreeVector err = ( *(hc) )[ihit]->GetPosErr();

            if (!(*(hc))[ihit]->IsGhost()) {
                fPosX += pos[0];
                fPosY += pos[1];
                fPosZ += pos[2];
                fErrX += err[0];
                fErrY += err[1];
                fErrZ += err[2];
                ngoodhits++;
            } else {
                fGhostPosX += pos[0];
                fGhostPosY += pos[1];
                fGhostPosZ += pos[2];
                fGhostErrX += err[0];
                fGhostErrY += err[1];
                fGhostErrZ += err[2];
                nghosthits++;
            }
        }
    }
    fPosX /= double(ngoodhits);
    fPosY /= double(ngoodhits);
    fPosZ /= double(ngoodhits);
    fErrX /= double(ngoodhits);
    fErrY /= double(ngoodhits);
    fErrZ /= double(ngoodhits);

    fGhostPosX /= double(nghosthits);
    fGhostPosY /= double(nghosthits);
    fGhostPosZ /= double(nghosthits);
    fGhostErrX /= double(nghosthits);
    fGhostErrY /= double(nghosthits);
    fGhostErrZ /= double(nghosthits);



    // G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    // if (pVVisManager)
    // {
    //     G4Square square(G4ThreeVector(fPosX, fPosY, fPosZ));
    //     square.SetScreenSize(6);
    //     square.SetFillStyle(G4Square::filled);
    //     G4Colour colour(1., 0., 0.);
    //     G4VisAttributes attribs(colour);
    //     square.SetVisAttributes(attribs);
    //     pVVisManager->Draw(square);

    //     G4Square square2(G4ThreeVector(fGhostPosX, fGhostPosY, fGhostPosZ));
    //     square2.SetScreenSize(6);
    //     square2.SetFillStyle(G4Square::filled);
    //     G4Colour colour2(1., 0., 0.);
    //     G4VisAttributes attribs2(colour2);
    //     square2.SetVisAttributes(attribs2);
    //     pVVisManager->Draw(square2);
    // }



    // Register Trigger State
    fHasInfo = nhits > 0.0;
    fEnergy  = 0.0;

    if (!fSave) return false;

    // If Triggered then fill
    if (fHasInfo) {

        // Fill muon vectors
        G4AnalysisManager* man = G4AnalysisManager::Instance();

        man->FillNtupleDColumn(fTimeIndex, fTime);
        man->FillNtupleDColumn(fPosXIndex, fPosX);
        man->FillNtupleDColumn(fPosYIndex, fPosY);
        man->FillNtupleDColumn(fPosZIndex, fPosZ);
        man->FillNtupleDColumn(fErrXIndex, fErrX);
        man->FillNtupleDColumn(fErrYIndex, fErrY);
        man->FillNtupleDColumn(fErrZIndex, fErrZ);

        return true;
    } else {

        // Set default values
        G4AnalysisManager* man = G4AnalysisManager::Instance();

        man->FillNtupleDColumn(fEdepIndex, -999.);
        man->FillNtupleDColumn(fTimeIndex, -999.);
        man->FillNtupleDColumn(fPosXIndex, -999.);
        man->FillNtupleDColumn(fPosYIndex, -999.);
        man->FillNtupleDColumn(fPosZIndex, -999.);
        man->FillNtupleDColumn(fErrXIndex, -999.);
        man->FillNtupleDColumn(fErrYIndex, -999.);
        man->FillNtupleDColumn(fErrZIndex, -999.);

        return false;
    }
}

void LongDriftProcessor::Reset() {
    VProcessor::Reset();
    fTime = 0.0;
    fEdep = 0.0;
    fPosX = 0.0;
    fPosY = 0.0;
    fPosZ = 0.0;
    fErrX = 0.0;
    fErrY = 0.0;
    fErrZ = 0.0;
    fGhostPosX = 0.0;
    fGhostPosY = 0.0;
    fGhostPosZ = 0.0;
    fGhostErrX = 0.0;
    fGhostErrY = 0.0;
    fGhostErrZ = 0.0;
}


} // - namespace COSMIC

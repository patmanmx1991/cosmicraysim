#include "AWEDriftSD.hh"
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
AWEDriftSD::AWEDriftSD(DBTable tbl):
    VDetector(tbl.GetIndexName(), "awedrift")
{
    std::cout << "DET: Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    fEfficiency = tbl.Has("efficiency") ? tbl.GetD("efficiency") : 1.0;
    fResolution = tbl.Has("resolution") ? tbl.GetG4D("resolution") : 2 * mm;

    fDetectorSizeX = 0.0;
    fDetectorSizeY = 0.0;
    fDetectorSizeZ = 0.0;
    fHCID = -1;

    fWirePosition = tbl.Has("wire_position") ? tbl.GetG4D("wire_position") : -999.9;

    collectionName.push_back(GetID());

    // By default also include the auto processor
    if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
        Analysis::Get()->RegisterProcessor(new AWEDriftProcessor(this));
    }

}

AWEDriftSD::AWEDriftSD(std::string name, std::string id,
                       bool autoprocess, bool autosave):
    VDetector(name, id)
{
    std::cout << "DET: Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // By default also include the auto processor
    if (autoprocess) {
        Analysis::Get()->RegisterProcessor(new AWEDriftProcessor(this, autosave));
    }

    fEfficiency = 1.0;
    fResolution = 2 * mm;

    fDetectorSizeX = 0.0;
    fDetectorSizeY = 0.0;
    fDetectorSizeZ = 0.0;
    fHCID = -1;

    fWirePosition = -999.9;

    collectionName.push_back(GetID());

    // By default also include the auto processor
    if (autoprocess) {
        Analysis::Get()->RegisterProcessor(new AWEDriftProcessor(this, autosave));
    }
}

void AWEDriftSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new DriftChamberHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4int AWEDriftSD::GetHCID() {
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(GetID());
    return fHCID;
}

G4int AWEDriftSD::GetGhostHCID() {
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(GetID());
    return fGhostHCID;
}

void AWEDriftSD::ResetState() {
    VDetector::ResetState();

    fEfficiencyThrow = G4UniformRand();

    if (fResolution > 0.0) {
        fResolutionThrow = G4RandGauss::shoot(0.0, fResolution);
    } else {
        fResolutionThrow = 0.0;
    }

}

G4bool AWEDriftSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/)
{
    // Remove hits based on efficiency for this event
    if (fEfficiency < fEfficiencyThrow) return false;

    // Get total energy deposted to check struck
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    // Get charge of particle, if neutral skip.
    G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
    if (charge == 0.) return false;

    // Get point before charge deposited...
    G4StepPoint* preStepPoint = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();

    // Get information used to transform to world geo
    G4TouchableHistory* touchable
        = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
    G4VPhysicalVolume* motherPhysical = touchable->GetVolume(1); // mother
    G4int copyNo = motherPhysical->GetCopyNo();

    // Get Translations between local and world geometry
    G4AffineTransform trans_worldtolocal = touchable->GetHistory()->GetTopTransform();

    // Get Local and world positions
    G4ThreeVector worldPos = 0.5 * (preStepPoint->GetPosition() + postStepPoint->GetPosition());
    G4ThreeVector localPos = trans_worldtolocal.TransformPoint(worldPos);

    // Depending on the detector, we must restrict location in the local geometry
    G4ThreeVector localPosP = localPos;
    G4ThreeVector localPosE;

    // Restrict to DOF in Y
    localPosP[0] = 0.0 * mm;
    localPosE[0] = fDetectorSizeX;

    localPosE[1] = fResolution;

    localPosP[2] = 0.0 * mm;
    localPosE[2] = fDetectorSizeZ;

    // Throw the position in Y according to the error
    localPosP[1] = localPosP[1] + fResolutionThrow;

    // World volume seems to be the local point
    G4AffineTransform trans_localtoworld = trans_worldtolocal.Invert();

    // Convert restricted data back to world geometry
    G4ThreeVector worldPosP = trans_localtoworld.TransformPoint(localPosP);
    G4ThreeVector worldPosE = trans_localtoworld.TransformPoint(localPosE);
    worldPosE -= trans_localtoworld.TransformPoint(G4ThreeVector());
    worldPosE[0] = fabs(worldPosE[0]);
    worldPosE[1] = fabs(worldPosE[1]);
    worldPosE[2] = fabs(worldPosE[2]);

    // Push back a hit for the true hit
    DriftChamberHit* hit = new DriftChamberHit(copyNo);
    hit->SetTime(preStepPoint->GetGlobalTime());
    hit->SetWorldPosTrue(worldPos);
    hit->SetWorldPos(worldPosP);
    hit->SetWorldPosErr(worldPosE);
    hit->SetLocalPosTrue(localPos);
    hit->SetLocalPos(localPosP);
    hit->SetLocalPosErr(localPosE);
    hit->SetGhost(false);
    fHitsCollection->insert(hit);

    // std::cout << GetID() << std::endl;
    // std::cout << "World Before : " << worldPos[0] / m  << " " << worldPos[1] / m  << " " << worldPos[2] / m << std::endl;
    // std::cout << "Local Before : " << localPos[0] / m  << " " << localPos[1] / m  << " " << localPos[2] / m << std::endl;
    // std::cout << "World After  : " << worldPosP[0] / m << " " << worldPosP[1] / m << " " << worldPosP[2] / m << std::endl;
    // std::cout << "World Error  : " << worldPosE[0] / m << " " << worldPosE[1] / m << " " << worldPosE[2] / m << std::endl;
    // std::cout << "Local After  : " << localPosP[0] / m << " " << localPosP[1] / m << " " << localPosP[2] / m << std::endl;
    // std::cout << "Local Error  : " << localPosE[0] / m << " " << localPosE[1] / m << " " << localPosE[2] / m << std::endl;



    // Always Add a ghost hit according to wire drift
    G4double updateY = fWirePosition  - (localPosP[1] - fWirePosition);
    if (fabs(updateY) < fDetectorSizeY) {
        localPosP[1] = updateY;
        worldPosP = trans_localtoworld.TransformPoint(localPosP);
    }
    hit = new DriftChamberHit(copyNo);
    hit->SetTime(preStepPoint->GetGlobalTime());
    hit->SetWorldPosTrue(worldPos);
    hit->SetWorldPos(worldPosP);
    hit->SetWorldPosErr(worldPosE);
    hit->SetLocalPosTrue(localPos);
    hit->SetLocalPos(localPosP);
    hit->SetLocalPosErr(localPosE);
    hit->SetGhost(true);
    fHitsCollection->insert(hit);

    return true;
}

void AWEDriftSD::DrawAll() {

    std::cout << "DET: Drawing all in Sensitive Detector" << std::endl;
    int nhits = (int) fHitsCollection->GetSize();
    for (int ihit = 0; ihit < nhits; ihit++) {
        ( *(fHitsCollection) )[ihit]->Draw();
    }
}

void AWEDriftSD::SetLogicalVolume(G4LogicalVolume* logic, G4VPhysicalVolume* physical) {
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
AWEDriftProcessor::AWEDriftProcessor(AWEDriftSD* trkr, bool autosave) :
    VProcessor(trkr->GetID()), fSave(autosave)
{
    fDetector = trkr;
    fHCID = trkr->GetHCID();
}

bool AWEDriftProcessor::BeginOfRunAction(const G4Run* /*run*/)
{
    if (fSave) {
        std::string tableindex = GetID();
        std::cout << "Registering AWEDriftSD NTuples " << tableindex << std::endl;

        G4AnalysisManager* man = G4AnalysisManager::Instance();

        // Fill index energy
        fTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");

        // fPosXIndex = man ->CreateNtupleDColumn(tableindex + "_lx");
        fPosYIndex = man ->CreateNtupleDColumn(tableindex + "_ly");
        // fPosZIndex = man ->CreateNtupleDColumn(tableindex + "_lz");

        // fErrXIndex = man ->CreateNtupleDColumn(tableindex + "_ex");
        fErrYIndex = man ->CreateNtupleDColumn(tableindex + "_ey");
        // fErrZIndex = man ->CreateNtupleDColumn(tableindex + "_ez");

        // fWorldPosXIndex = man ->CreateNtupleDColumn(tableindex + "_wx");
        // fWorldPosYIndex = man ->CreateNtupleDColumn(tableindex + "_wy");
        // fWorldPosZIndex = man ->CreateNtupleDColumn(tableindex + "_wz");

        // fWorldErrXIndex = man ->CreateNtupleDColumn(tableindex + "_wex");
        // fWorldErrYIndex = man ->CreateNtupleDColumn(tableindex + "_wey");
        // fWorldErrZIndex = man ->CreateNtupleDColumn(tableindex + "_wez");

        // fGhostPosXIndex = man ->CreateNtupleDColumn(tableindex + "_glx");
        fGhostPosYIndex = man ->CreateNtupleDColumn(tableindex + "_gly");
        // fGhostPosZIndex = man ->CreateNtupleDColumn(tableindex + "_glz");

        // fGhostWorldPosXIndex = man ->CreateNtupleDColumn(tableindex + "_gwx");
        // fGhostWorldPosYIndex = man ->CreateNtupleDColumn(tableindex + "_gwy");
        // fGhostWorldPosZIndex = man ->CreateNtupleDColumn(tableindex + "_gwz");

        std::cout << "Registering AWEDriftSD NTuples " << tableindex << " : " << fTimeIndex << std::endl;

    }
    Reset();
    return true;
}

bool AWEDriftProcessor::ProcessEvent(const G4Event* event)
{

    // Get the hit collection
    fHCID = fDetector->GetHCID();
    DriftChamberHitsCollection* hc =
        static_cast<DriftChamberHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));

    // Get hit count for this SD
    int nhits = (int)  (hc)->GetSize();
    if (nhits < 1) {
        return FillNTuples();
    }

    // Get the time of the true primary vertex
    G4double T = (event->GetPrimaryVertex())->GetT0();

    // Keep track of total ghost/nonghost for averages
    int ngoodhits = 0;
    int nghosthits = 0;

    // Loop over all hits
    for (int ihit = 0; ihit < nhits; ihit++) {

        // Save time of each hit to get average in ns
        fTime += (( ( *(hc) )[ihit]->GetTime() / ns) - T / ns);

        G4ThreeVector pos = ( *(hc) )[ihit]->GetLocalPos();
        G4ThreeVector postrue = ( *(hc) )[ihit]->GetLocalPosTrue();
        G4ThreeVector wpos = ( *(hc) )[ihit]->GetWorldPos();
        G4ThreeVector wpostrue = ( *(hc) )[ihit]->GetWorldPosTrue();

        G4ThreeVector err = ( *(hc) )[ihit]->GetLocalPosErr();
        G4ThreeVector werr = ( *(hc) )[ihit]->GetWorldPosErr();

        // Save info depending on if its a ghost
        if (!(*(hc))[ihit]->IsGhost()) {

            fPosX += pos[0];
            fPosY += pos[1];
            fPosZ += pos[2];

            fErrX += err[0];
            fErrY += err[1];
            fErrZ += err[2];

            fPosTrueX += postrue[0];
            fPosTrueY += postrue[1];
            fPosTrueZ += postrue[2];

            fWorldPosX += wpos[0];
            fWorldPosY += wpos[1];
            fWorldPosZ += wpos[2];

            fWorldErrX += werr[0];
            fWorldErrY += werr[1];
            fWorldErrZ += werr[2];

            fWorldPosTrueX += wpostrue[0];
            fWorldPosTrueY += wpostrue[1];
            fWorldPosTrueZ += wpostrue[2];

            ngoodhits++;

        } else {

            fGhostPosX += pos[0];
            fGhostPosY += pos[1];
            fGhostPosZ += pos[2];

            fGhostWorldPosX += wpos[0];
            fGhostWorldPosY += wpos[1];
            fGhostWorldPosZ += wpos[2];

            nghosthits++;
        }
    }

    // Calculate hit averages
    fTime /= double(ngoodhits + nghosthits);

    fPosX /= double(ngoodhits);
    fPosY /= double(ngoodhits);
    fPosZ /= double(ngoodhits);
    fPosTrueX /= double(ngoodhits);
    fPosTrueY /= double(ngoodhits);
    fPosTrueZ /= double(ngoodhits);

    fErrX /= double(ngoodhits);
    fErrY /= double(ngoodhits);
    fErrZ /= double(ngoodhits);

    fWorldPosX /= double(ngoodhits);
    fWorldPosY /= double(ngoodhits);
    fWorldPosZ /= double(ngoodhits);
    fWorldPosTrueX /= double(ngoodhits);
    fWorldPosTrueY /= double(ngoodhits);
    fWorldPosTrueZ /= double(ngoodhits);

    fWorldErrX /= double(ngoodhits);
    fWorldErrY /= double(ngoodhits);
    fWorldErrZ /= double(ngoodhits);

    fGhostPosX /= double(nghosthits);
    fGhostPosY /= double(nghosthits);
    fGhostPosZ /= double(nghosthits);
    fGhostWorldPosX /= double(nghosthits);
    fGhostWorldPosY /= double(nghosthits);
    fGhostWorldPosZ /= double(nghosthits);

    // Register Trigger State
    fHasInfo = ngoodhits > 0;
    fEnergy  = 0.0;

    return FillNTuples();
}

bool AWEDriftProcessor::FillNTuples() {

    if (!fSave) return false;
    G4AnalysisManager* man = G4AnalysisManager::Instance();

    if (fHasInfo) {

        man->FillNtupleDColumn(fTimeIndex, fTime);

        // man->FillNtupleDColumn(fPosXIndex, fPosX);
        man->FillNtupleDColumn(fPosYIndex, fPosY);
        // man->FillNtupleDColumn(fPosZIndex, fPosZ);

        // man->FillNtupleDColumn(fErrXIndex, fErrX);
        man->FillNtupleDColumn(fErrYIndex, fErrY);
        // man->FillNtupleDColumn(fErrZIndex, fErrZ);

        // man->FillNtupleDColumn(fWorldPosXIndex, fWorldPosX);
        // man->FillNtupleDColumn(fWorldPosYIndex, fWorldPosY);
        // man->FillNtupleDColumn(fWorldPosZIndex, fWorldPosZ);

        // man->FillNtupleDColumn(fWorldErrXIndex, fWorldErrX);
        // man->FillNtupleDColumn(fWorldErrYIndex, fWorldErrY);
        // man->FillNtupleDColumn(fWorldErrZIndex, fWorldErrZ);

        // man->FillNtupleDColumn(fGhostPosXIndex, fGhostPosX);
        man->FillNtupleDColumn(fGhostPosYIndex, fGhostPosY);
        // man->FillNtupleDColumn(fGhostPosZIndex, fGhostPosZ);

        // man->FillNtupleDColumn(fGhostWorldPosXIndex, fGhostWorldPosX);
        // man->FillNtupleDColumn(fGhostWorldPosYIndex, fGhostWorldPosY);
        // man->FillNtupleDColumn(fGhostWorldPosZIndex, fGhostWorldPosZ);

        return true;

    } else {

        man->FillNtupleDColumn(fTimeIndex, -999.);

        // man->FillNtupleDColumn(fPosXIndex, -999.);
        man->FillNtupleDColumn(fPosYIndex, -999.);
        // man->FillNtupleDColumn(fPosZIndex, -999.);

        // man->FillNtupleDColumn(fErrXIndex, -999.);
        man->FillNtupleDColumn(fErrYIndex, -999.);
        // man->FillNtupleDColumn(fErrZIndex, -999.);

        // man->FillNtupleDColumn(fWorldPosXIndex, -999.);
        // man->FillNtupleDColumn(fWorldPosYIndex, -999.);
        // man->FillNtupleDColumn(fWorldPosZIndex, -999.);

        // man->FillNtupleDColumn(fWorldErrXIndex, -999.);
        // man->FillNtupleDColumn(fWorldErrYIndex, -999.);
        // man->FillNtupleDColumn(fWorldErrZIndex, -999.);

        // man->FillNtupleDColumn(fGhostPosXIndex, -999.);
        man->FillNtupleDColumn(fGhostPosYIndex, -999.);
        // man->FillNtupleDColumn(fGhostPosZIndex, -999.);

        // man->FillNtupleDColumn(fGhostWorldPosXIndex, -999.);
        // man->FillNtupleDColumn(fGhostWorldPosYIndex, -999.);
        // man->FillNtupleDColumn(fGhostWorldPosZIndex, -999.);

        return false;
    }
}

void AWEDriftProcessor::Reset() {
    VProcessor::Reset();

    fTime = 0.0;

    fPosX = 0.0;
    fPosY = 0.0;
    fPosZ = 0.0;

    fPosTrueX = 0.0;
    fPosTrueY = 0.0;
    fPosTrueZ = 0.0;

    fErrX = 0.0;
    fErrY = 0.0;
    fErrZ = 0.0;

    fWorldPosX = 0.0;
    fWorldPosY = 0.0;
    fWorldPosZ = 0.0;

    fWorldPosTrueX = 0.0;
    fWorldPosTrueY = 0.0;
    fWorldPosTrueZ = 0.0;

    fWorldErrX = 0.0;
    fWorldErrY = 0.0;
    fWorldErrZ = 0.0;

    fGhostPosX = 0.0;
    fGhostPosY = 0.0;
    fGhostPosZ = 0.0;

    fGhostWorldPosX = 0.0;
    fGhostWorldPosY = 0.0;
    fGhostWorldPosZ = 0.0;
}

} // - namespace COSMIC

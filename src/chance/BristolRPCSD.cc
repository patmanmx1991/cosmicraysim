#include "BristolRPCSD.hh"

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
#include "G4Navigator.hh"

#include "analysis/Analysis.hh"
#include "db/DBTable.hh"
#include "sd/DriftHit.hh"
#include "geo/GeoManager.hh"

namespace COSMIC {

//------------------------------------------------------------------
BristolRPCSD::BristolRPCSD(DBTable tbl):
    VDetector(tbl.GetIndexName(), "bristolrpc")
{
    std::cout << "DET: Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    fEfficiency = tbl.Has("efficiency") ? tbl.GetD("efficiency") : 1.0;

    fResolution = tbl.Has("resolution") ? tbl.GetG4D("resolution") : 0.400 * mm;

    fDetectorSizeX = 0.0;
    fDetectorSizeY = 0.0;
    fDetectorSizeZ = 0.0;
    fHCID = -1;

    collectionName.push_back(GetID());

    // By default also include the auto processor
    if (!tbl.Has("processor") or tbl.GetI("processor") > 0) {
        Analysis::Get()->RegisterProcessor(new BristolRPCProcessor(this));
    }

}

BristolRPCSD::BristolRPCSD(std::string name, std::string id,
                           bool autoprocess, bool autosave):
    VDetector(name, id)
{
    std::cout << "DET: Creating new " << GetType()
              << " detector : " << GetID() << std::endl;

    // Set initial state
    ResetState();

    // By default also include the auto processor
    if (autoprocess) {
        Analysis::Get()->RegisterProcessor(new BristolRPCProcessor(this, autosave));
    }

    fResolution = 0.400 * mm;

    fDetectorSizeX = 0.0;
    fDetectorSizeY = 0.0;
    fDetectorSizeZ = 0.0;
    fHCID = -1;

    collectionName.push_back(GetID());
}

void BristolRPCSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new DriftChamberHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(fHCID, fHitsCollection);
}

G4int BristolRPCSD::GetHCID() {
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(GetID());
    return fHCID;
}

void BristolRPCSD::ResetState() {
    VDetector::ResetState();
    fEfficiencyThrow = G4UniformRand();

    if (fResolution > 0.0) {
        fResolutionThrow = G4RandGauss::shoot(0.0, fResolution);
    } else {
        fResolutionThrow = 0.0;
    }
}

G4bool BristolRPCSD::ProcessHits(G4Step* step, G4TouchableHistory* /*touch*/)
{
    if (fEfficiency < fEfficiencyThrow) return false;

    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    G4double charge = step->GetTrack()->GetDefinition()->GetPDGCharge();
    if (charge == 0.) return false;


    G4StepPoint* preStepPoint  = step->GetPreStepPoint();
    G4StepPoint* postStepPoint = step->GetPostStepPoint();

    G4TouchableHistory* touchable
        = (G4TouchableHistory*)(step->GetPreStepPoint()->GetTouchable());
    G4VPhysicalVolume* motherPhysical = touchable->GetVolume(1); // mother
    G4int copyNo = motherPhysical->GetCopyNo();

    // Get Translations between local and world geometry
    G4AffineTransform trans_worldtolocal = touchable->GetHistory()->GetTopTransform();

    // Get Local and world positions. 
    G4ThreeVector worldPos = 0.5*(preStepPoint->GetPosition()+postStepPoint->GetPosition());
    G4ThreeVector localPos = trans_worldtolocal.TransformPoint(worldPos);

    // Depending on the detector, we must restrict location in the local geometry
    G4ThreeVector localPosP = localPos;
    G4ThreeVector localPosE;

    // Restrict to DOF in XY, Z at center of SD
    localPosP[0] = 0.0 * mm;
    localPosE[0] = fDetectorSizeX;

    localPosE[1] = fResolution;

    localPosP[2] = 0.0 * mm;
    localPosE[2] = fDetectorSizeZ;

    // Apply local smearing to XY
    localPosP[1] = localPos[1] + fResolutionThrow;

    // World volume seems to be the local point
    G4AffineTransform trans_localtoworld = trans_worldtolocal.Invert();
    G4ThreeVector worldPosP = trans_localtoworld.TransformPoint(localPosP);

    // Get error after world pos rotation
    G4ThreeVector worldPosE = trans_localtoworld.TransformPoint(localPosE);
    worldPosE -= trans_localtoworld.TransformPoint(G4ThreeVector());
    worldPosE[0] = fabs(worldPosE[0]);
    worldPosE[1] = fabs(worldPosE[1]);
    worldPosE[2] = fabs(worldPosE[2]);

    // std::cout << GetID() << std::endl;
    // std::cout << "World Before : " << worldPos[0] / m  << " " << worldPos[1] / m  << " " << worldPos[2] / m << std::endl;
    // std::cout << "Local Before : " << localPos[0] / m  << " " << localPos[1] / m  << " " << localPos[2] / m << std::endl;
    // std::cout << "World After  : " << worldPosP[0] / m << " " << worldPosP[1] / m << " " << worldPosP[2] / m << std::endl;
    // std::cout << "World Error  : " << worldPosE[0] / m << " " << worldPosE[1] / m << " " << worldPosE[2] / m << std::endl;
    // std::cout << "Local After  : " << localPosP[0] / m << " " << localPosP[1] / m << " " << localPosP[2] / m << std::endl;
    // std::cout << "Local Error  : " << localPosE[0] / m << " " << localPosE[1] / m << " " << localPosE[2] / m << std::endl;


    // Add hit to collection
    DriftChamberHit* hit = new DriftChamberHit(copyNo);

    hit->SetTime(preStepPoint->GetGlobalTime());

    hit->SetWorldPos(worldPosP);
    hit->SetWorldPosTrue(worldPos);
    hit->SetWorldPosErr(worldPosE);

    hit->SetLocalPos(localPosP);
    hit->SetLocalPosTrue(localPos);
    hit->SetLocalPosErr(localPosE);

    fHitsCollection->insert(hit);

    return true;
}

void BristolRPCSD::DrawAll() {

    std::cout << "DET: Drawing all in Sensitive Detector" << std::endl;
    int nhits = (int) fHitsCollection->GetSize();
    for (int ihit = 0; ihit < nhits; ihit++) {
        ( *(fHitsCollection) )[ihit]->Draw();
    }

}

void BristolRPCSD::SetLogicalVolume(G4LogicalVolume* logic, G4VPhysicalVolume* physical) {
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
BristolRPCProcessor::BristolRPCProcessor(BristolRPCSD* trkr, bool autosave) :
    VProcessor(trkr->GetID()), fSave(autosave)
{
    fDetector = trkr;
    fHCID = trkr->GetHCID();
}

bool BristolRPCProcessor::BeginOfRunAction(const G4Run* /*run*/) {

    if (fSave) {
        std::string tableindex = GetID();
        std::cout << "Registering BristolRPCProcessor NTuples " << tableindex << std::endl;

        G4AnalysisManager* man = G4AnalysisManager::Instance();

        fTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
        fPosYIndex = man ->CreateNtupleDColumn(tableindex + "_ly");
        fErrYIndex = man ->CreateNtupleDColumn(tableindex + "_ey");
    }

    Reset();
    return true;
}

bool BristolRPCProcessor::ProcessEvent(const G4Event* event) {

    // Get hit collection for this event/sd
    fHCID = fDetector->GetHCID();
    DriftChamberHitsCollection* hc =
        static_cast<DriftChamberHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));

    // Check some hits added
    int nhits = (int)  (hc)->GetSize();
    if (nhits < 1) {
        return FillNTuples();
    }

    // Get the time of this events primary vertex
    G4double T = (event->GetPrimaryVertex())->GetT0();
    int ngoodhits = 0;

    // Calculate averages based on all hits in event
    for (int ihit = 0; ihit < nhits; ihit++) {

        fTime += (( ( *(hc) )[ihit]->GetTime() / ns) - T / ns);

        G4ThreeVector pos = ( *(hc) )[ihit]->GetLocalPos();
        fPosX += pos[0];
        fPosY += pos[1];
        fPosZ += pos[2];

        G4ThreeVector truepos = ( *(hc) )[ihit]->GetLocalPosTrue();
        fPosTrueX += truepos[0];
        fPosTrueY += truepos[1];
        fPosTrueZ += truepos[2];

        G4ThreeVector err = ( *(hc) )[ihit]->GetLocalPosErr();
        fErrX += err[0];
        fErrY += err[1];
        fErrZ += err[2];

        G4ThreeVector wpos = ( *(hc) )[ihit]->GetWorldPos();
        fWorldPosX += wpos[0];
        fWorldPosY += wpos[1];
        fWorldPosZ += wpos[2];

        G4ThreeVector truewpos = ( *(hc) )[ihit]->GetWorldPosTrue();
        fWorldPosTrueX += truewpos[0];
        fWorldPosTrueY += truewpos[1];
        fWorldPosTrueZ += truewpos[2];

        G4ThreeVector werr = ( *(hc) )[ihit]->GetWorldPosErr();
        fWorldErrX += werr[0];
        fWorldErrY += werr[1];
        fWorldErrZ += werr[2];

        ngoodhits++;
    }

    // Get averages from sums
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

    // Register Trigger State
    fHasInfo = nhits > 0.0;
    fEnergy  = 0.0;

    // Check if NTuples need filling
    return FillNTuples();

}

bool BristolRPCProcessor::FillNTuples() {

    if (!fSave) return false;
    G4AnalysisManager* man = G4AnalysisManager::Instance();

    if (fHasInfo) {

        man->FillNtupleDColumn(fTimeIndex, fTime);
        man->FillNtupleDColumn(fPosYIndex, fPosY);
        man->FillNtupleDColumn(fErrYIndex, fErrY);

        return true;

    } else {

        man->FillNtupleDColumn(fTimeIndex, -999.);
        man->FillNtupleDColumn(fPosYIndex, -999.);
        man->FillNtupleDColumn(fErrYIndex, -999.);

        return false;
    }
}



void BristolRPCProcessor::Reset() {
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
}


} // - namespace COSMIC

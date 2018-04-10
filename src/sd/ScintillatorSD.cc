//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B5HodoscopeSD.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5HodoscopeSD.cc
/// \brief Implementation of the B5HodoscopeSD class

#include "ScintillatorSD.hh"
#include "ScintillatorHit.hh"

#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4Neutron.hh"
#include "G4Gamma.hh"
#include "G4Proton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "Randomize.hh"


namespace COSMIC {
ScintillatorSD::ScintillatorSD(DBLink* tbl)
    : DetectorObject(tbl)
{
    std::cout << "Creating new Scintillator SD " << std::endl;
    fTable   = tbl;
    fType    = fTable->GetS("type");
    fTableIndex    = fTable->GetIndexName() + "_" + fType;
    collectionName.push_back(fTableIndex);
}

ScintillatorSD::~ScintillatorSD() {
}


void ScintillatorSD::Initialize(G4HCofThisEvent* hce)
{
    fHitsCollection = new ScintillatorHitsCollection(SensitiveDetectorName, collectionName[0]);
    if (fHCID < 0) fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(fHCID, fHitsCollection);
}


bool ScintillatorSD::BeginOfRunAction(const G4Run* run) {
    std::cout << "Registering ScintillatorSD NTuples " << fTableIndex << std::endl;

    G4AnalysisManager* man = G4AnalysisManager::Instance();

    // Fill index energy
    fEdepIndex = man ->CreateNtupleDColumn(fTableIndex + "_E");
    fTimeIndex = man ->CreateNtupleDColumn(fTableIndex + "_t");
    fPosXIndex = man ->CreateNtupleDColumn(fTableIndex + "_x");
    fPosYIndex = man ->CreateNtupleDColumn(fTableIndex + "_y");
    fPosZIndex = man ->CreateNtupleDColumn(fTableIndex + "_z");
    fThXZIndex = man ->CreateNtupleDColumn(fTableIndex + "_thXZ");
    fThYZIndex = man ->CreateNtupleDColumn(fTableIndex + "_thYZ");

    Reset();
    return true;
}

bool ScintillatorSD::RecordEvent(const G4Event* event) {

    // Set default values
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fEdepIndex, -999.);
    man->FillNtupleDColumn(fTimeIndex, -999.);
    man->FillNtupleDColumn(fPosXIndex, -999.);
    man->FillNtupleDColumn(fPosYIndex, -999.);
    man->FillNtupleDColumn(fPosZIndex, -999.);
    man->FillNtupleDColumn(fThXZIndex, -999.);
    man->FillNtupleDColumn(fThYZIndex, -999.);
    Reset();

    // Average over hits
    ScintillatorHitsCollection* hc = static_cast<ScintillatorHitsCollection*> (event->GetHCofThisEvent()->GetHC(fHCID));
    int nhits = (int)  (hc)->GetSize();
    if (nhits < 1) {
        return false;
    }

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

    // If Energy deposited then fill
    if (fEdep > 0.) {
        // Fill muon vectors
        man->FillNtupleDColumn(fEdepIndex, fEdep);
        man->FillNtupleDColumn(fTimeIndex, fTime);
        man->FillNtupleDColumn(fPosXIndex, fPosX);
        man->FillNtupleDColumn(fPosYIndex, fPosY);
        man->FillNtupleDColumn(fPosZIndex, fPosZ);
        man->FillNtupleDColumn(fThXZIndex, fThetaXZ);
        man->FillNtupleDColumn(fThYZIndex, fThetaYZ);

        Reset();
        return true;
    } else {
        Reset();
        return false;
    }
}

void ScintillatorSD::Reset() {
    fTime = 0.0;
    fEdep = 0.0;
    fPosX = 0.0;
    fPosY = 0.0;
    fPosZ = 0.0;
    fThetaXZ = 0.0;
    fThetaYZ = 0.0;
}

G4bool ScintillatorSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{

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

    fHitsCollection->insert(hit);

    return true;
}

}
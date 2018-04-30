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
// $Id: DriftChamberHit.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file DriftChamberHit.cc
/// \brief Implementation of the DriftChamberHit class

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<DriftChamberHit>* DriftChamberHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DriftChamberHit::DriftChamberHit()
    : G4VHit(), fLayerID(-1), fTime(0.), fLocalPos(0), fWorldPos(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DriftChamberHit::DriftChamberHit(G4int z)
    : G4VHit(), fLayerID(z), fTime(0.), fLocalPos(0), fWorldPos(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DriftChamberHit::~DriftChamberHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DriftChamberHit::DriftChamberHit(const DriftChamberHit &right)
    : G4VHit() {
    fLayerID = right.fLayerID;
    fWorldPos = right.fWorldPos;
    fLocalPos = right.fLocalPos;
    fTime = right.fTime;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const DriftChamberHit& DriftChamberHit::operator=(const DriftChamberHit &right)
{
    fLayerID = right.fLayerID;
    fWorldPos = right.fWorldPos;
    fLocalPos = right.fLocalPos;
    fTime = right.fTime;
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int DriftChamberHit::operator==(const DriftChamberHit &/*right*/) const
{
    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DriftChamberHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager)
    {
        // Build G4Box from Errors
        G4Box hitbox =  G4Box("hitbox", fabs(fWorldPosErr[0]), fabs(fWorldPosErr[1]), fabs(fWorldPosErr[2]));
        G4Transform3D tr = G4Transform3D(G4RotationMatrix(), fWorldPos);
        G4Colour colour(0., 1., 0.);
        G4VisAttributes attribs(colour);
        pVVisManager->Draw(hitbox, attribs, tr);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::map<G4String, G4AttDef>* DriftChamberHit::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String, G4AttDef>* store
        = G4AttDefStore::GetInstance("DriftChamberHit", isNew);

    if (isNew) {
        (*store)["HitType"]
            = G4AttDef("HitType", "Hit Type", "Physics", "", "G4String");

        (*store)["ID"]
            = G4AttDef("ID", "ID", "Physics", "", "G4int");

        (*store)["Time"]
            = G4AttDef("Time", "Time", "Physics", "G4BestUnit", "G4double");

        (*store)["Pos"]
            = G4AttDef("Pos", "Position", "Physics", "G4BestUnit", "G4ThreeVector");
    }
    return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue>* DriftChamberHit::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;

    values
    ->push_back(G4AttValue("HitType", "DriftChamberHit", ""));
    values
    ->push_back(G4AttValue("ID", G4UIcommand::ConvertToString(fLayerID), ""));
    values
    ->push_back(G4AttValue("Time", G4BestUnit(fTime, "Time"), ""));
    values
    ->push_back(G4AttValue("Pos", G4BestUnit(fWorldPos, "Length"), ""));

    return values;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// void DriftChamberHit::Print() const
// {
//     G4cout << "  Layer[" << fLayerID << "] : time " << fTime / ns
//            << " (nsec) --- local (x,y) " << fLocalPos.x()
//            << ", " << fLocalPos.y() << G4endl;
// }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

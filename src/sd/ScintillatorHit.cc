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
// $Id: B5HodoscopeHit.cc 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5HodoscopeHit.cc
/// \brief Implementation of the B5HodoscopeHit class

#include "ScintillatorHit.hh"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator=0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintillatorHit::ScintillatorHit() : G4VHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintillatorHit::~ScintillatorHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ScintillatorHit::ScintillatorHit(const ScintillatorHit &right)
: G4VHit() {
    fId = right.fId;
    fTime = right.fTime;
    fEdep = right.fEdep;
    fType = right.fType;
    fPos = right.fPos;
    fRot = right.fRot;
   // fPLogV = right.fPLogV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const ScintillatorHit& ScintillatorHit::operator=(const ScintillatorHit &right)
{
    fId = right.fId;
    fTime = right.fTime;
    fEdep = right.fEdep;
    fType = right.fType;
    fPos = right.fPos;
    fRot = right.fRot;
    //fPLogV = right.fLogV;
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int ScintillatorHit::operator==(const ScintillatorHit &/*right*/) const
{
    return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/*void ScintillatorHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager)
    {
        G4Transform3D trans(fRot.inverse(),fPos);
        G4VisAttributes attribs;
        const G4VisAttributes* pVA = fPLogV->GetVisAttributes();
        if (pVA) attribs = *pVA;
        G4Colour colour(0.,1.,1.);
        attribs.SetColour(colour);
        attribs.SetForceSolid(true);
        pVVisManager->Draw(*fPLogV,attribs,trans);
    }
}*/

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
/*
const std::map<G4String,G4AttDef>* ScintillatorHit::GetAttDefs() const
{
    G4bool isNew;
    std::map<G4String,G4AttDef>* store
    = G4AttDefStore::GetInstance("B5ScintillatorHit",isNew);

    if (isNew) {
        (*store)["HitType"]
          = G4AttDef("HitType","Hit Type","Physics","","G4String");

        (*store)["ID"]
          = G4AttDef("ID","ID","Physics","","G4int");

        (*store)["Time"]
          = G4AttDef("Time","Time","Physics","G4BestUnit","G4double");

        (*store)["Pos"]
          = G4AttDef("Pos","Position","Physics","G4BestUnit","G4ThreeVector");

        (*store)["LVol"]
          = G4AttDef("LVol","Logical Volume","Physics","","G4String");
    }
    return store;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4AttValue>* ScintillatorHit::CreateAttValues() const
{
    std::vector<G4AttValue>* values = new std::vector<G4AttValue>;

    values
      ->push_back(G4AttValue("HitType","ScintillatorHit",""));
    values
      ->push_back(G4AttValue("ID",G4UIcommand::ConvertToString(fId),""));
    values
      ->push_back(G4AttValue("Time",G4BestUnit(fTime,"Time"),""));
    values
      ->push_back(G4AttValue("Pos",G4BestUnit(fPos,"Length"),""));

    if (fPLogV)
        values->push_back(G4AttValue("LVol",fPLogV->GetName(),""));
    else
        values->push_back(G4AttValue("LVol"," ",""));

    return values;
}
*/
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ScintillatorHit::Print()
{
//    G4cout << "  Hodoscope[" << fId << "] " << fTime/ns << " (nsec)" << G4endl;
G4cout << G4endl;
G4cout << G4endl;
G4cout << G4endl;
    G4cout << "  Hit : " << particle_type << " Time : " << fTime/ns << " (nsec)" << " Pos. : " << fPos/m << G4endl;
G4cout << G4endl;
G4cout << G4endl;
G4cout << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


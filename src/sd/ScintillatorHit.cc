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
#include "G4Square.hh"
#include "G4Colour.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

G4ThreadLocal G4Allocator<ScintillatorHit>* ScintillatorHitAllocator = 0;

ScintillatorHit::ScintillatorHit() : G4VHit()
{}

ScintillatorHit::~ScintillatorHit()
{}

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

void ScintillatorHit::SetAngles(G4ThreeVector dir) {
    fThetaXZ = std::atan2(dir.getZ(), dir.getX());
    fThetaYZ = std::atan2(dir.getZ(), dir.getY());
}

int ScintillatorHit::operator==(const ScintillatorHit &/*right*/) const
{
    return 0;
}

void ScintillatorHit::Draw()
{
    G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
    if (pVVisManager)
    {
        G4Square square(fPos);
        square.SetScreenSize(6);
        square.SetFillStyle(G4Square::filled);
        G4Colour colour(1., 0., 0.);
        G4VisAttributes attribs(colour);
        square.SetVisAttributes(attribs);
        pVVisManager->Draw(square);
    }
}

void ScintillatorHit::Print()
{
    G4cout << "  Hodoscope[" << fId << "] " << fTime / ns << " (nsec)" << G4endl;
    G4cout << G4endl;
    G4cout << G4endl;
    G4cout << G4endl;
    G4cout << "  Hit : " << particle_type << " Time : " << fTime / ns << " (nsec)" << " Pos. : " << fPos / m << G4endl;
    G4cout << G4endl;
    G4cout << G4endl;
    G4cout << G4endl;
}

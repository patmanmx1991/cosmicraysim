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
// $Id: B5HodoscopeSD.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file B5HodoscopeSD.hh
/// \brief Definition of the B5HodoscopeSD class

#ifndef ScintillatorSD_h
#define ScintillatorSD_h

#include "G4VSensitiveDetector.hh"
#include "ScintillatorHit.hh"

#include "DetectorObject.hh"
#include "G4UserRunAction.hh"
#include "globals.hh"

#include "G4Run.hh"
#include "globals.hh"
#include "g4root.hh"


class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

namespace COSMIC {


class ScintillatorSD : public DetectorObject {
public:
  ScintillatorSD(DBLink* table);
  ~ScintillatorSD();

  bool BeginOfRunAction(const G4Run* run);
  bool RecordEvent(const G4Event* event);
  virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void Reset();

  virtual void Initialize(G4HCofThisEvent*HCE);


  ScintillatorHitsCollection* fHitsCollection;
  G4int fHCID;


  DBLink* fTable;

  std::string fLogicalName;

  std::string fType;
  std::string fTableIndex;
  std::string fLogical;

  G4double      fTime;
  G4double      fEdep;
  G4double      fPosX;
  G4double      fPosY;
  G4double      fPosZ;
  G4double      fThetaXZ;
  G4double      fThetaYZ;

  int fTimeIndex;
  int fPosXIndex;
  int fPosYIndex;
  int fPosZIndex;
  int fThXZIndex;
  int fThYZIndex;
  int fEdepIndex;


};

}
#endif

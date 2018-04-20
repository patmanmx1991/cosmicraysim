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
// $Id: CosmicRunAction.hh 70599 2013-06-03 11:14:56Z gcosmo $
//
/// \file CosmicRunAction.hh
/// \brief Definition of the CosmicRunAction class

#ifndef CosmicRunAction_h
#define CosmicRunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "sd/DetectorManager.hh"
class G4Run;

/// User's CosmicRunAction class. this class implements all the user actions to be executed at each run

class CosmicRunAction : public G4UserRunAction
{
  public:
  /// constructor
    CosmicRunAction();
  /// destructor
    virtual ~CosmicRunAction();
    
    virtual G4Run* GenerateRun();
  /// Called at the beginning of each run
    virtual void BeginOfRunAction(const G4Run*);
  /// Called at the end of each run
    virtual void   EndOfRunAction(const G4Run*);
  int fCurrentRun;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


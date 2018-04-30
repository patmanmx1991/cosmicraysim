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
// $Id: CosmicRun.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file CosmicRun.cc
/// \brief Implementation of the CosmicRun class

#include "action/CosmicRun.hh"
// #include "sd/CosmicHits.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "sd/DetectorManager.hh"
#include "geo/GeoManager.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"
#include "sd/DetectorManager.hh"
#include "trigger/TriggerManager.hh"
#include "analysis/Analysis.hh"
using namespace COSMIC;

CosmicRun::CosmicRun()
  : G4Run()
{
  fPrintSize = Analysis::Get()->GetChunkSize() / 10.0;
}

CosmicRun::~CosmicRun()
{
}

void CosmicRun::RecordEvent(const G4Event* event)
{
  // Logging
  int eventid = event->GetEventID();
  if (eventid % fPrintSize == 0) {
    std::cout << "RUN: --> Processing Event : " << eventid
              << "/" << numberOfEventToBeProcessed << std::endl;
  }

  Analysis::Get()->ProcessEvent(event);

  // Reset analysis state for next event.
  Analysis::Get()->BeginOfEventAction();
}


void CosmicRun::Merge(const G4Run* aRun)
{
  // const CosmicRun* localRun = static_cast<const CosmicRun*>(aRun);
  G4Run::Merge(aRun);
}


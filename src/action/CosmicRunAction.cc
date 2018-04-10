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
// $Id: CosmicRunAction.cc 71323 2013-06-13 16:54:23Z gcosmo $
//
/// \file CosmicRunAction.cc
/// \brief Implementation of the CosmicRunAction class

#include "action/CosmicRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"

#include "sd/DetectorManager.hh"
#include "sd/DetectorManager.hh"
#include "action/CosmicAnalysis.hh"
#include "action/CosmicRun.hh"
#include "action/CosmicAnalysis.hh"

using namespace COSMIC;

CosmicRunAction::CosmicRunAction()
 : G4UserRunAction()
{       
}

CosmicRunAction::~CosmicRunAction()
{
  delete G4AnalysisManager::Instance();
}

G4Run* CosmicRunAction::GenerateRun()
{ return new CosmicRun; }

void CosmicRunAction::BeginOfRunAction(const G4Run* run)
{ 
  G4cout << "### Run " << run->GetRunID() << " start." << G4endl;
  
  // Create analysis manager
  // Notice: it must be done the same way in master and workers
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->SetVerboseLevel(1);
  man->SetFirstNtupleId(0);
  man->SetFirstHistoId(0);
  int ntupleindex = 0;
  int histogindex = 0;

  // Creating Ntuples
  G4cout << " --> Creating Dynamic Detector Ntuples" << std::endl;
  man->CreateNtuple("detectorevents", "Dynamic Detector Saved Information");

  // Iterate through all the sensitive detectors and call the functions to read info
  if (DetectorManager::Get()->GetNDetectors() > 0){

    // Get pointers to all detectors
    bool saverow = DetectorManager::Get()->BeginOfRunAction(run, ntupleindex, histogindex);

    // If any detectors are loaded they should have added stuff to the NTUPLE
    if (saverow) man->FinishNtuple();
    man->FinishNtuple();
  }

  // Save random seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);

  // Create the output file  
  G4cout << " --> Opening output : " << DB::Get()->GetOutputFile() << std::endl;
  man->OpenFile(DB::Get()->GetOutputFile());

}

void CosmicRunAction::EndOfRunAction(const G4Run* run)
{
  //retrieve the number of events produced in the run
  G4int nofEvents = run->GetNumberOfEvent();

  //do nothing, if no events were processed
  if (nofEvents == 0) return;
 
  //print
  //
  if (IsMaster())
  {
    G4cout
     << "\n--------------------End of Global Run-----------------------"
     << " \n The run was " << nofEvents << " events ";
  }
  else
  {
    G4cout
     << "\n--------------------End of Local Run------------------------"
     << " \n The run was " << nofEvents << " ";
  }      

  // const ShuklaPrimaryGenerator* generatorAction = static_cast<const ShuklaPrimaryGenerator*>(
  //       G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());
  // if (generatorAction) {
  //   G4ParticleGun* gun = generatorAction -> GetParticleGun();

  //   // Register the flux generator information, PDG, momentum, position

  //   partName = particle->GetParticleName();
  // }
  // {
  //     // The GetParticleGun() is defined inside the B3PrimaryGeneratorAction.hh file and
  //     // it returns a pointer to the current concrete implementation of the G4VPrimaryGeneratorAction
  //     // Note that GetParticleDefinition return a 'const' type in the case of the use of the G4ParticleGun but not
  //     // in the case of the G4ParticleGeneralSource. Hence the GetParticleGun in the B3PrimaryGeneratorAction.hh
  //     // must not be defined 'const' in the case of the GeneralParticleSource
  //     //
  //   G4ParticleDefinition* particle = generatorAction -> GetParticleGun() -> GetParticleDefinition();
  //   partName = particle->GetParticleName();
  // }  
  



  // Save everything to file
  std::cout << "Writing analysis manager instance" << std::endl;
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->Write();
  man->CloseFile();
}


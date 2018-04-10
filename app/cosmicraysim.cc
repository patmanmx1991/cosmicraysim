/**
 * @file rat.cc
 * @author Stan Seibert <volsung@physics.utexas.edu>
 */

#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>

#include <G4UImanager.hh>
#include <G4UIterminal.hh>
#include <G4UItcsh.hh>
#include <Randomize.hh>
#include <globals.hh>
#include <G4VisExecutive.hh>
#include <G4UIExecutive.hh>

// time.h must come after Randomize.hh on RedHat 7.3
#include <time.h>

#include "db/DB.hh"
#include "sd/DetectorConstruction.hh"
#include "action/CosmicActionInitialization.hh"
#include "physics/PhysicsFactory.hh"
#include <TStopwatch.h>
#include <TRandom.h>
#include <iostream>
#include <string>


#include "G4UImanager.hh"

#include "Randomize.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#include "g4root_defs.hh"
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif
#include "globals.hh"
#include "action/CosmicRun.hh"

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


#include "G4UserRunAction.hh"
#include "globals.hh"


using namespace std;
using namespace COSMIC;

std::vector<std::string> gGeomtryFiles;
std::string gNEvents = "-1";
std::string gOutputFile = "";
std::string gMacroFile = "";
bool gInteractive = false;

int main(int argc, char** argv) {

  // Print Splash Screen
  DB::PrintSplashScreen();


  // initialize RATDB and load all .ratdb files in $GLG4DATA,
  // usually set to $RATROOT/data
  DB *rdb = DB::Get();
  if ( getenv("GLG4DATA") != NULL ) rdb->Load(string(getenv("GLG4DATA")));
  else rdb->Load(string("data"));

  // Now that the main database is setup we want to give users the option to update the DB
  // inside their macros. This is important because we want to save the DB into the output
  // so that settings are tracked across runs.
  for (int i = 1; i < argc; ++i) {
    std::cout << "TRYING INPUT : " << i << std::endl;
    if (std::strcmp(argv[i], "-g") == 0) {
      std::vector<std::string> geoms = DBUtils::Expand(std::string(argv[++i]));
      for (uint j = 0; j < geoms.size(); j++) {
        gGeomtryFiles.push_back(geoms[j]);
      }
    } else if (std::strcmp(argv[i], "-n") == 0) {
      gNEvents = std::string(argv[++i]);
    } else if (std::strcmp(argv[i], "-o") == 0) {
      gOutputFile = std::string(argv[++i]);
    } else if (std::strcmp(argv[i], "-m") == 0) {
      gMacroFile = std::string(argv[++i]);
    } else if (std::strcmp(argv[i], "-i") == 0) {
      gInteractive = true;
    } else {
      std::cout << "Unknown COMMAND : " << argv[i] << std::endl;
      throw;
    }
  }
  if (gGeomtryFiles.size() > 0) {
    for (uint i = 0; i < gGeomtryFiles.size(); i++) {
      rdb->LoadFile(gGeomtryFiles[i]);
    }
  }
  rdb->Finalise();


  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);

  // Construct the default run manager. Pick the proper run
  // manager depending if the multi-threading option is
  // active or not.
#ifdef G4MULTITHREADED
  G4MTRunManager* runManager = new G4MTRunManager;
#else
  G4RunManager* runManager = new G4RunManager;
#endif
  // Set mandatory initialization classes
  runManager->SetUserInitialization(new DetectorConstruction);
  runManager->SetUserInitialization(PhysicsFactory::LoadPhysicsList());
  runManager->SetUserInitialization(new CosmicActionInitialization());

  // Initialize G4 kernel
  runManager->Initialize();

#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Check valid output
  if (gOutputFile.empty()) {
    std::cout << "Need a valid output file : '-o outputfile.root' " << std::endl;
    throw;
  }

  rdb->SetOutputFile(gOutputFile);





  // Possible user modes
  // - Interactive
  // - Macro
  // - Jobs

  // INTERACTIVE
  if (gInteractive) {
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);

#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute init_vis.mac");
#else
    UImanager->ApplyCommand("/control/execute init.mac");
#endif
    // start the session here: make the Geant4 prompt Idle>
    // available to the user
    ui->SessionStart();
    delete ui;
#endif
#ifdef G4VIS_USE
    delete visManager;
#endif


    // MACRO
  } else if (!gMacroFile.empty()) { // batch mode

    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command + gMacroFile);


    // BATCH
  } else {

    // Check N EVENTS
    if (std::stoi(gNEvents) < 1) {
      std::cout << "Need a number of events to generate : '-n 1000'" << std::endl;
      throw;
    }

    // // Get the default runcommands
    DBLink* tbl = rdb->GetLink("GLOBAL", "config");
    std::string commands = tbl->GetS("batchcommands");

    // std::vector<std::string> coms = tbl->GetFields();
    // for (uint i = 0; i < coms.size(); i++){
    //   std::cout << coms[i] << std::endl;
    //   UImanager->ApplyCommand(tbl->GetS(coms[i]));
    // }

    // Now run the beam
    UImanager->ApplyCommand("/run/beamOn " + gNEvents);
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !
  delete runManager;

  return 0;
}


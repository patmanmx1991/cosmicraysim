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
//#ifdef G4MULTITHREADED
//#include "G4MTRunManager.hh"
//#else
#include "G4RunManager.hh"
//#endif

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

#include <ctime>
#include <iostream>

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "analysis/Analysis.hh"

using namespace std;
using namespace COSMIC;


// Run Inputs
std::string gRunTag = "cosmicoutput";
int gRunID = 0;
int gSubRunID = 0;

long int gSeed = -1; ///< Seed. -1 means generate from time+pid

/// Mode : Interactive
bool gInteractive = false; //< Run interactive viewer

// Mode : generate N events
int gNEvents = -1; ///< Number of events to generate regardless of trigger

// Mode : generate N triggers
int gNTriggers = -1; ///< Number of triggered events to generate

// Mode : Generate t exposure
int gExposureTime = -1; ///< Number of seconds exposure to generate

int gProcessingChunks = 10000; ///< By default what the chunk size should be

// User Defined Geometry files
std::vector<std::string> gGeomtryFiles;

///-------------------------------------------------------------------------------------------
void PrintHelpScreen() {

  std::cout << "USAGE" << std::endl << std::endl;

  std::cout << " -n nevents : Events to generate" << std::endl;
  std::cout << " -j ntriggs : Triggers to generate" << std::endl;
  std::cout << " -t exposur : Exposure to generate" << std::endl;
  std::cout << " -c chunksz : Event chunk size for exposure/trigger mode" << std::endl;
  std::cout << " -s seed    : Seed. Default is -1, meaning get from time+pid" << std::endl;
  std::cout << " -o outtag  : Output File Tag. Will create file : outtag.run.subrun.root " << std::endl;
  std::cout << " -i         : Flag. Run in interactive mode." << std::endl;
  std::cout << " -g geofile : Load a geometry JSON file. Can use multiple times." << std::endl;
  std::cout << " --run    r : Set Run ID Manually" << std::endl;
  std::cout << " --subrun r : Set Sub Run ID Manually" << std::endl << std::endl;

  exit(0);

}


///-------------------------------------------------------------------------------------------
int main(int argc, char** argv) {

  // Print Splash Screen
  DB::PrintSplashScreen();

  // Get User Inputs
  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Getting User Inputs" << std::endl;

  // Loop over all arguments
  for (int i = 1; i < argc; ++i) {

    // N Events input " -n nevents"
    if (std::strcmp(argv[i], "-n") == 0) {
      gNEvents = std::stoi(argv[++i]);

      // N Triggers Input " -j ntriggers"
    } else if (std::strcmp(argv[i], "-j") == 0) {
      gNTriggers = std::stoi(argv[++i]);

      // Exposure Time Input " -t exposure"
    } else if (std::strcmp(argv[i], "-t") == 0) {
      gExposureTime = std::stoi(argv[++i]);

      // Chunk Input " -c chunksize"
    } else if (std::strcmp(argv[i], "-c") == 0) {
      gProcessingChunks = std::stoi(argv[++i]);

      // Seed Input " -s seedval"
    } else if (std::strcmp(argv[i], "-s") == 0) {
      gSeed = std::stol(argv[++i]);

      // Seed Input " -s seedval"
    } else if (std::strcmp(argv[i], "--run") == 0) {
      gRunID = std::stoi(argv[++i]);

      // Seed Input " -s seedval"
    } else if (std::strcmp(argv[i], "--subrun") == 0) {
      gSubRunID = std::stoi(argv[++i]);

      // Run Tag " -s outputtag"
    } else if (std::strcmp(argv[i], "-o") == 0) {
      gRunTag = std::string(argv[++i]);

      // Interactive Mode Flag "-i"
    } else if (std::strcmp(argv[i], "-i") == 0) {
      gInteractive = true;

      // Geometry input " -g geofile1 -g geofile2 "
    } else if (std::strcmp(argv[i], "-g") == 0) {
      std::vector<std::string> geoms = DBUtils::Expand(std::string(argv[++i]));
      for (uint j = 0; j < geoms.size(); j++) {
        gGeomtryFiles.push_back(geoms[j]);
      }

      // Print Splash Screen
    } else if (std::strcmp(argv[i], "-h") == 0) {
      PrintHelpScreen();


      // Uknown command
    } else {
      std::cout << "Unknown COMMAND : " << argv[i] << std::endl;
      throw;
    }
  }


// Setup Seed
  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Getting Seed Info " << std::endl;

// Choose the Random engine
  CLHEP::RanecuEngine* rand = new CLHEP::RanecuEngine;
  if (gSeed < 0) {
    std::cout << "APP: --> Getting seed from time+pid." << std::endl;
    long tl = std::time(0);
    long pid = getpid();
    std::cout << "APP: --> pid : " << pid << std::endl;
    std::cout << "APP: --> time : " << tl << std::endl;
    gSeed = int( tl + (pid + 1.));
    std::cout << "APP: --> seed : " << gSeed << std::endl;
  } else {
    std::cout << "APP: --> User specified seed. " << std::endl;
    std::cout << "APP: --> seed = " << gSeed << std::endl;
  }

  rand->setSeed(gSeed);
  G4Random::setTheEngine(rand);


// Setup the Database
  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Loading Default Database " << std::endl;
  DB *rdb = DB::Get();
  if ( getenv("GLG4DATA") != NULL ) rdb->Load(string(getenv("GLG4DATA")));
  else rdb->Load(string("data"));

  if (gGeomtryFiles.size() > 0) {
    for (uint i = 0; i < gGeomtryFiles.size(); i++) {
      rdb->LoadFile(gGeomtryFiles[i]);
    }
  }
  rdb->Finalise();
  std::cout << "========================================= " << std::endl;

  std::cout << "APP: Setting up Geant4 Run Manager " << std::endl;
// Construct the default run manager. Pick the proper run
// manager depending if the multi-threading option is
// active or not.
//#ifdef G4MULTITHREADED
//  G4MTRunManager* runManager = new G4MTRunManager;
//#else
  G4RunManager* runManager = new G4RunManager;
  //#endif


  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Setting User Initialization" << std::endl;
// Set mandatory initialization classes
  runManager->SetUserInitialization(new DetectorConstruction);
  runManager->SetUserInitialization(PhysicsFactory::LoadPhysicsList());
  runManager->SetUserInitialization(new CosmicActionInitialization());

// Initialize G4 kernel
  runManager->Initialize();

#ifdef G4VIS_USE
// Initialize visualization
  G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

// Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Assigning Output Information" << std::endl;
  std::cout << "APP: --> Output Tag       : " << gRunTag   << std::endl;
  std::cout << "APP: --> Output Run ID    : " << gRunID    << std::endl;
  std::cout << "APP: --> Sub Run Start ID : " << gSubRunID << std::endl;
  Analysis::Get()->SetOutputTag(gRunTag);
  Analysis::Get()->SetOutputRun(gRunID);
  Analysis::Get()->SetOutputSubRun(gSubRunID);


// ----------------------------------------------------------------------
// INTERACTIVE MODE
// ----------------------------------------------------------------------
  if (gInteractive) {

    std::cout << "========================================= " << std::endl;
    std::cout << "APP: Running Interactive Mode" << std::endl;

#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);

#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute " + DB::GetDataPath() + "/init_vis.mac");
#else
    UImanager->ApplyCommand("/control/execute " + DB::GetDataPath() + "/init.mac");
#endif
    // start the session here: make the Geant4 prompt Idle>
    // available to the user
    ui->SessionStart();
    delete ui;
#endif
#ifdef G4VIS_USE
    delete visManager;
#endif


    // ----------------------------------------------------------------------
    // Batch Processing
    // ----------------------------------------------------------------------
  } else {

    std::cout << "========================================= " << std::endl;
    std::cout << "APP: Running Batch Mode" << std::endl;

    // Run initial configuration commands
    DBTable tbl = rdb->GetTable("GLOBAL", "config");
    if (tbl.Has("batchcommands")) {

      // Get users pre macro file
      std::string commands = tbl.GetS("batchcommands");

      // Execute the macro
      if (!commands.empty()) {
        G4String command = "/control/execute ";
        UImanager->ApplyCommand(command + commands);
      }
    }


    // *********************************
    // N Generated mode
    // *********************************
    if (gNEvents > 0 && gNTriggers < 0 && gExposureTime < 0) {
      std::cout << "========================================= " << std::endl;
      std::cout << "APP: Running Generator" << std::endl;
      std::cout << "APP: --> NEvents : " << gNEvents << std::endl << std::endl;



      std::cout << "APP: Starting Run : " << gNEvents << std::endl;
      UImanager->ApplyCommand("/run/beamOn " + std::to_string(gNEvents));

      // *********************************
      // N Triggers mode
      // *********************************
    } else if (gNEvents < 0 && gNTriggers > 0 && gExposureTime < 0) {

      std::cout << "========================================= " << std::endl;
      std::cout << "APP: Running NTriggers" << std::endl;
      std::cout << "APP: --> Desired Triggers : " << gNTriggers << std::endl;
      std::cout << "APP: --> Processing Chunk : " << gProcessingChunks << std::endl << std::endl;
      Analysis::Get()->SetChunkSize(gProcessingChunks);

      // Run first loop for estimate
      long int curtime = time(0);
      UImanager->ApplyCommand("/run/beamOn " + std::to_string(gProcessingChunks));
      int singletriggers = Analysis::Get()->GetNSavedEvents();
      long int timetaken = time(0) - curtime;

      // Print Estimated Loops
      std::cout << "APP: " << gProcessingChunks << " events generated : "
                << singletriggers << " s of exposure" << std::endl;

      std::cout << "APP: Loops for full exposure : " << float(gNTriggers) / singletriggers << std::endl;

      std::cout << "APP: CPU time for full exposure : "
                << timetaken * float(gNTriggers) / singletriggers / 60. << " minutes" <<  std::endl;


      // Infinite loop until total triggers reached
      while (singletriggers < gNTriggers) {

        // Run Loop Again
        UImanager->ApplyCommand("/run/beamOn " + std::to_string(gProcessingChunks));

        // Get new exposure time
        singletriggers = Analysis::Get()->GetNSavedEvents();

      }

      std::cout << "Acquired above desired triggers : " << singletriggers << std::endl;

      // *********************************
      // Exposure Time Mode
      // *********************************
    } else if (gNEvents < 0 && gNTriggers < 0 && gExposureTime > 0) {

      std::cout << "========================================= " << std::endl;
      std::cout << "APP: Running Exposure" << std::endl;
      std::cout << "APP: --> Desired Exposure : " << gExposureTime << " s" << std::endl;
      std::cout << "APP: --> Processing Chunk : " << gProcessingChunks << std::endl << std::endl;
      Analysis::Get()->SetChunkSize(gProcessingChunks);

      // Run first loop for estimate
      long int curtime = time(0);
      UImanager->ApplyCommand("/run/beamOn " + std::to_string(gProcessingChunks));
      double singleexp = Analysis::Get()->GetExposureTime();
      long int timetaken = time(0) - curtime;

      // Print Estimated Loops
      std::cout << "APP: " << gProcessingChunks << " events generated : "
                << singleexp << " s of exposure" << std::endl;

      std::cout << "APP: Loops for full exposure : " << gExposureTime / singleexp << std::endl;

      std::cout << "APP: CPU time for full exposure : "
                << timetaken * gExposureTime / singleexp / 60. << " minutes" <<  std::endl;

      // Infinite loop until total exposure reached
      while (singleexp < gExposureTime) {

        // Run Loop Again
        UImanager->ApplyCommand("/run/beamOn " + std::to_string(gProcessingChunks));

        // Get new exposure time
        singleexp = Analysis::Get()->GetExposureTime();

      }

      std::cout << "Acquired above desired exposure : " << singleexp << std::endl;


      // *********************************
      // User Error
      // *********************************
    }  else {

      std::cout << "Can only run one of the nevents/ntriggers/exposure at once in batch mode!" << std::endl;
      std::cout << "NEvents   : -n nevents" << std::endl;
      std::cout << "NTriggers : -l ntriggers" << std::endl;
      std::cout << "Exposure  : -t exposure" << std::endl;
      throw;
    }
  }

  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Cleaning Up!" << std::endl;

// Job termination
// Free the store: user actions, physics_list and detector_description are
// owned and deleted by the run manager, so they should not be deleted
// in the main() program !
  delete runManager;

  std::cout << "========================================= " << std::endl;


  return 0;
}


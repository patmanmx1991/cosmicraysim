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

#include "trackfitting/TrackFitter.hh"
#include "trackfitting/BristolPoCAFitter.hh"

using namespace std;
using namespace COSMIC;


// Run Inputs
std::string gRunTag = "trackfit";
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

std::string gInputFile;
std::string gOutputTag = "trackfit";

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
    if (std::strcmp(argv[i], "-i") == 0) {
      gInputFile = std::string(argv[++i]);

      // N Triggers Input " -j ntriggers"
    } else if (std::strcmp(argv[i], "-o") == 0) {
      gOutputTag = std::string(argv[++i]);

      // Print Splash Screen
    } else if (std::strcmp(argv[i], "-h") == 0) {
      PrintHelpScreen();

      // Uknown command
    } else {
      std::cout << "Unknown COMMAND : " << argv[i] << std::endl;
      throw;
    }
  }

  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Beginning Input Loop" << std::endl;

  // Read in input Trees
  TFile* f = new TFile(gInputFile.c_str(),"READ");
  TTree* t = (TTree*)f->Get("detectorevents");

  BristolSingleTrackFitter* trfit_below = new BristolSingleTrackFitter();
  trfit_below->ReadInputTTree(t);

  BristolSingleTrackFitter* trfit_above = new BristolSingleTrackFitter();
  trfit_above->ReadInputTTree(t, "system_above");

  BristolPoCAFitter* pocafit = new BristolPoCAFitter();
  pocafit->ReadInputTTree(t, "system","system_above");

  // Get Truth Information
  double system_volume_px;
  double system_volume_py;
  double system_volume_pz;
  t->SetBranchAddress("system_volume_px",&system_volume_px);
  t->SetBranchAddress("system_volume_py",&system_volume_py);
  t->SetBranchAddress("system_volume_pz",&system_volume_pz);

  // Do a single track fit with the lower detector and a track fit with upper detector.
  // Save In Out information and Truth Tree



  // Make output File
  TFile* o = new TFile((gInputFile + gOutputTag + ".root").c_str(),"RECREATE");
  TTree* otree = new TTree("trackfit","trackfit");
  trfit_below->AddToOutputTTree(otree);
  trfit_above->AddToOutputTTree(otree,"above");
  pocafit->AddToOutputTTree(otree,"poca");

  for (int i = 0; i < t->GetEntries(); i++){
    t->GetEntry(i);
    int status = trfit_below->FitTracks();
    status = trfit_above->FitTracks();
    status = pocafit->FitTracks();

    otree->Fill();
  }

  o->cd();
  otree->Write();
  o->Close();






  // Setup track fitter for each TFile
  // Get Entry
  // Run Track Fit
  // If valid track fill the TTree
  // If valid MC save the MC truth object
  // Close outputfile and TTree
  // Move onto next one.









  return 0;
}


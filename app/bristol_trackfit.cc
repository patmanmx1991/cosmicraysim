/**
 * @file rat.cc
 * @author Stan Seibert <volsung@physics.utexas.edu>
 */
#include "Math/Functor.h"
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "sd/LongDriftSD.hh"
#include "sd/SimpleScintillatorSD.hh"
#include "TH1.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TVirtualFitter.h"
#include "TSystem.h"
#include "TStopwatch.h"
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

#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
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
#include "G4Polyline.hh"
#include "Math/Functor.h"

#include "db/ROOTHeaders.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"
#include "sd/DetectorManager.hh"
#include "sd/LongDriftSD.hh"
#include "sd/AWEDriftSD.hh"
#include "sd/BristolRPCSD.hh"



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
#include "TVector3.h"

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
void FillComboVect(std::vector<std::vector<bool> >& combomap, int n) {

  for (int i = 0; i < (1 << n); i++) {
    std::vector<bool> combo;
    for (int j = 0; j < n; j++) {
      combo.push_back(((i & (1 << j)) ? 1 : 0));
    }
    combomap.push_back(combo);
  }

}

double GetAverage(std::vector<double>* co) {
  double avg = 0.0;
  for (int i = 0; i < co->size(); i++) {
    avg += co->at(i);
  }
  return avg;
}

double GetAverage(std::vector<double>* co1, std::vector<double>* co2) {
  double avg = 0.0;
  for (int i = 0; i < co1->size(); i++) {
    avg += co1->at(i);
  }
  for (int i = 0; i < co2->size(); i++) {
    avg += co2->at(i);
  }
  return avg;
}

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
  std::cout << "APP: Reading Configuration" << std::endl;
  bool fUseRPC;
  bool fUseDrift;
  uint fMinARPCX;
  uint fMinARPCY;
  uint fMinBRPCX;
  uint fMinBRPCY;
  uint fMinADriftX;
  uint fMinADriftY;
  uint fMinBDriftX;
  uint fMinBDriftY;

  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Beginning Input Loop" << std::endl;

  // Read in input Trees
  TFile* f = new TFile(gInputFile.c_str(), "READ");
  TTree* t = (TTree*)f->Get("detectorevents");

  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Reading Event Inputs" << std::endl;

  std::string prefixa = "system_above";
  std::string prefixb = "system";

  BristolPoCAFitter* pocafit = new BristolPoCAFitter();
  pocafit->ReadInputTTree(t, "system", "system_above");

  // Make output File
  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Creating Output Tree" << std::endl;

  TFile* o = new TFile((gInputFile + gOutputTag + ".root").c_str(), "RECREATE");
  TTree* otree = new TTree("trackfit", "trackfit");

  double fBestFitPars[7];
  double fCovarMatrix[49];
  double fMinuitParams[6];
  double fErrorDef;

  Double_t fScattering[31] = {0.};
  Double_t fPOCAScattering[6] = {0.};
  Double_t fMCTruth[7] = {0.};

  std::string prefix = "fit";

  std::string bestfitparslab = prefix + "_trackfit";
  std::string bestfitparsval = "x/D:y:z:px1:py1:px2:py2";
  otree->Branch(bestfitparslab.c_str(), fBestFitPars, bestfitparsval.c_str() );

  std::string minuitparslab = prefix + "_minuitparams";
  std::string minuitparsval = "fmin/D:edm:errdef:nvpar:nparx:istat";
  otree->Branch(minuitparslab.c_str(), fMinuitParams, minuitparsval.c_str());

  std::string covariancelab = prefix + "_covar";
  std::string covarianceval = "";
  covarianceval += "c00/D:c01:c02:c03:c04:c05:c06:";
  covarianceval += "c10:c11:c12:c13:c14:c15:c16:";
  covarianceval += "c20:c21:c22:c23:c24:c25:c26:";
  covarianceval += "c30:c31:c32:c33:c34:c35:c36:";
  covarianceval += "c40:c41:c42:c43:c44:c45:c46:";
  covarianceval += "c50:c51:c52:c53:c54:c55:c56:";
  covarianceval += "c60:c61:c62:c63:c64:c65:c66";
  otree->Branch(covariancelab.c_str(), fCovarMatrix, covarianceval.c_str());

  otree->Branch("fitscattering", fScattering, "scatteranglex/D:scatterangley:scatterangle3d:uoffsetx:uoffsety:loffsetx:loffsety:offset3d:vlongerror:vtranserror1:vtranserror2:vx:vy:vz:px1:px2:py1:py2:uxchi2:uychi2:lxchi2:lychi2:xchi2:ychi2:CFchi2:uxpoca:lxpoca:uypoca:lypoca:estp:estpcorr");
  otree->Branch("pocascattering", fPOCAScattering, "scatteranglex/D:scatterangley:scatterangle3d:vx:vy:vz");
  otree->Branch("mctruth", fMCTruth, "energy/D:p:px:py:pz:v:scatterangle:passflag");


  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Creating Histograms" << std::endl;

  // TH1F* cutHisto = new TH1F("cuthisto", "Number of events after each cut step", 13, 0, 13);
  // TH1F* pAccHisto = new TH1F("p_acc_histo","Momentum accuracy",500, -2, 30);
  // TH1F* pAccCorrHisto = new TH1F("p_acc_corr_histo","Momentum accuracy (corrected)",500, -2, 30);

  // TH2F* estVsTrueHisto = new TH2F("P_true_P_est", "Estimated Pagainst true P; P estimate; P true",100,-100,15000,100,-100,12000);
  // TH2F* sigmaVsTrueHisto = new TH2F("Sigma_P_true","Sigma against 1/P_true; 1/P_true; sigma",100,0,0.003,100,0,0.03);




  std::cout << "========================================= " << std::endl;
  std::cout << "APP: Creating Minimizer" << std::endl;



  std::vector<bool> combobx;
  std::vector<bool> comboby;
  std::vector<bool> comboay;
  std::vector<bool> comboax;

  // Loop over all entries
  for (int i = 0; i < t->GetEntries(); i++) {

    // Read Event
    t->GetEntry(i);

    if (i % 10000 == 0) std::cout << "Processed " << i << " events. " << std::endl;
    
    // Apply NHit cuts
    if (pocafit->GetNAboveRPCX()   < 3) continue;
    if (pocafit->GetNAboveRPCY()   < 3) continue;
    if (pocafit->GetNBelowRPCX()   < 3) continue;
    if (pocafit->GetNBelowRPCY()   < 3) continue;
    // if (pocafit->GetNAboveDriftX() < 3) continue;
    // if (pocafit->GetNAboveDriftY() < 3) continue;
    // if (pocafit->GetNBelowDriftX() < 3) continue;
    // if (pocafit->GetNBelowDriftY() < 3) continue;

    // std::cout << "Getting Combinations" << std::endl;

    // Get the best fit drift combinations for this event
    if (pocafit->GetNAboveDriftX()) {
      comboax = pocafit->GetBestComboForDriftHits(BristolPoCAFitter::kFitAllAboveX);
      pocafit->SetAboveComboX(&comboax);
    }
    if (pocafit->GetNAboveDriftY()) {
      comboay = pocafit->GetBestComboForDriftHits(BristolPoCAFitter::kFitAllAboveY);
      pocafit->SetAboveComboY(&comboay);
    }
    if (pocafit->GetNBelowDriftX()) {
      combobx = pocafit->GetBestComboForDriftHits(BristolPoCAFitter::kFitAllBelowX);
      pocafit->SetBelowComboX(&combobx);
    }
    if (pocafit->GetNBelowDriftY()) {
      comboby = pocafit->GetBestComboForDriftHits(BristolPoCAFitter::kFitAllBelowY);
      pocafit->SetBelowComboY(&comboby);
    }

    // std::cout << " Getting Start Predictions " << std::endl;
    pocafit->SetUseAll();

    // Get the starting track values for above and below
    double startpx1 = pocafit->PredictStartAbovePX();
    double startpy1 = pocafit->PredictStartAbovePY();
    double startpx2 = pocafit->PredictStartBelowPX();
    double startpy2 = pocafit->PredictStartBelowPY();

    // Set the starting PoCA as the mean
    double startx = pocafit->PredictStartPoCAX();
    double starty = pocafit->PredictStartPoCAY();
    double startz = pocafit->PredictStartPoCAZ();

    // std::cout << " Start : " << startx << " " << starty << " " << startz << std::endl;
    // std::cout << " Start P1 : " << startpx1 << " " << startpy1 << std::endl;
    // std::cout << " Start P2 : " << startpx2 << " " << startpy2 << std::endl;

    // Tell POCA to use all available information in subsequent fit
    pocafit->SetUseAll();

    // std::cout << "Creating minimizer" << std::endl;

    // Create Minimizer Object
    ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
    min->SetPrintLevel(-1);

    // Setup Functor for the fitter
    int npars = 7;
    BristolPoCAFitterFCN* fcn = new BristolPoCAFitterFCN(pocafit);
    ROOT::Math::Functor func(*fcn, npars);
    min->SetFunction(func);

    // Tell Minuit the variables
    min->SetVariable(0, "rx",  startx,   0.1);
    min->SetVariable(1, "ry",  starty,   0.1);
    min->SetVariable(2, "rz",  startz,   0.1);
    min->SetVariable(3, "px1", startpx1, 0.1);
    min->SetVariable(4, "py1", startpy1, 0.1);
    min->SetVariable(5, "px2", startpx2, 0.1);
    min->SetVariable(6, "py2", startpy2, 0.1);

    // std::cout << "Running Fit" << std::endl;
    // Run the fit
    min->Minimize();

    // Get results
    const double* xx = min->X();
    for (int j = 0; j < npars; j++) {
      fBestFitPars[j] = xx[j];
    }

    // Get Minuit Params
    Double_t istat = 0;
    if (min->CovMatrixStatus() > 0) istat = 3;
    fMinuitParams[0] = pocafit->DoEval(xx);
    fMinuitParams[1] = min->Edm();
    fMinuitParams[2] = min->ErrorDef();
    fMinuitParams[3] = 7;
    fMinuitParams[4] = 7;
    fMinuitParams[5] = istat;

    // Get the covariance
    min->GetCovMatrix(fCovarMatrix);

    // Read params for calculations
    double fitVx  = xx[0];
    double fitVy  = xx[1];
    double fitVz  = xx[2];
    double fitpx1 = xx[3];
    double fitpy1 = xx[4];
    double fitpx2 = xx[5];
    double fitpy2 = xx[6];

    // Get Scattering Angles
    TVector3 grad1X(fitpx1, 1., 1.);
    TVector3 grad2X(fitpx2, 1., 1.);

    TVector3 grad1Y(1., fitpy1, 1.);
    TVector3 grad2Y(1., fitpy2, 1.);

    TVector3 grad1(fitpx1, fitpy1, 1.);
    TVector3 grad2(fitpx2, fitpy2, 1.);

    fScattering[0] = grad1X.Angle(grad2X); // Scatter Angle X
    fScattering[1] = grad1Y.Angle(grad2Y); // Scatter Angle Y
    fScattering[2] = grad1.Angle(grad2); // Scatter Angle 3d

    // std::cout << "Getting offsets" << std::endl;
    // Get offsets entering the target volume
    // = fabs(hitPosition[2] - (fitVx + fitpx2*(zPosition[2] - fitVz)));
    double U_offsetX = fabs( pocafit->GetLowestXXAbove() - (fitVx + fitpx2 * (pocafit->GetLowestXZAbove() - fitVz)) );
    double U_offsetY = fabs( pocafit->GetLowestYYAbove() - (fitVy + fitpy2 * (pocafit->GetLowestYZAbove() - fitVz)) );
    double L_offsetX = fabs(pocafit->GetHighestXXBelow() - (fitVx + fitpy2 * (pocafit->GetHighestXZBelow() - fitVz)) );
    double L_offsetY = fabs(pocafit->GetHighestYYBelow() - (fitVy + fitpy2 * (pocafit->GetHighestYZBelow() - fitVz)) );

    fScattering[3] = U_offsetX; // Find lowest Z point in X
    fScattering[4] = U_offsetY; // Find lowest Z point in Y
    fScattering[5] = L_offsetX; // Find highest Z point in X
    fScattering[6] = L_offsetY; // Find highest Z point in Y
    pocafit->SetUseAll();

    // Unknown
    fScattering[7] = 0.0;

    // Get PoCA errors
    TMatrixD* covarV = new TMatrixD(7, 7, fCovarMatrix);
    covarV->ResizeTo(3, 3);

    TVector3 vec_w_tmp = grad1;
    vec_w_tmp += grad2;
    vec_w_tmp = vec_w_tmp.Unit();
    TVector3 vec_v_tmp = grad1.Cross(grad2);
    vec_v_tmp = vec_v_tmp.Unit();
    TVector3 vec_u_tmp = vec_w_tmp.Cross(vec_v_tmp);

    Double_t wXYZ[3] = {vec_w_tmp.X(), vec_w_tmp.Y(), vec_w_tmp.Z()};
    TVectorD vec_w(3, wXYZ);
    Double_t vXYZ[3] = {vec_v_tmp.X(), vec_v_tmp.Y(), vec_v_tmp.Z()};
    TVectorD vec_v(3, vXYZ);
    Double_t uXYZ[3] = {vec_u_tmp.X(), vec_u_tmp.Y(), vec_u_tmp.Z()};
    TVectorD vec_u(3, uXYZ);

    fScattering[8] = sqrt(covarV->Similarity(vec_w)); //V_long_error;
    fScattering[9] = sqrt(covarV->Similarity(vec_v)); //V_trans1_error;
    fScattering[10] = sqrt(covarV->Similarity(vec_u));  //V_trans2_error;


    // Get actual fit values again?!
    fScattering[11] = fitVx;
    fScattering[12] = fitVy;
    fScattering[13] = fitVz;
    fScattering[14] = fitpx1;
    fScattering[15] = fitpx2;
    fScattering[16] = fitpy1;
    fScattering[17] = fitpy2;


    // std::cout << "Getting Chi2 " << std::endl;

    // Save upper and lower chi2
    double chi2arpcx = pocafit->GetChi2AboveRPCX( fitVx, fitpx1, fitVz );
    double chi2arpcy = pocafit->GetChi2AboveRPCY( fitVy, fitpy1, fitVz );
    double chi2brpcx = pocafit->GetChi2BelowRPCX( fitVx, fitpx2, fitVz );
    double chi2brpcy = pocafit->GetChi2BelowRPCY( fitVy, fitpy2, fitVz );

    double chi2adriftx = pocafit->GetChi2AboveDriftX( fitVx, fitpx1, fitVz );
    double chi2adrifty = pocafit->GetChi2AboveDriftY( fitVy, fitpy1, fitVz );
    double chi2bdriftx = pocafit->GetChi2BelowDriftX( fitVx, fitpx2, fitVz );
    double chi2bdrifty = pocafit->GetChi2BelowDriftY( fitVy, fitpy2, fitVz );

    fScattering[18] = chi2arpcx + chi2adriftx; // event.xUchi2;
    fScattering[19] = chi2arpcy + chi2adrifty; // event.yUchi2;
    fScattering[20] = chi2brpcx + chi2bdriftx; // event.xLchi2;
    fScattering[21] = chi2brpcy + chi2bdrifty; // event.yLchi2;
    fScattering[22] = chi2arpcx + chi2adriftx + chi2brpcx + chi2bdriftx; // event.xchi2;
    fScattering[23] = chi2arpcy + chi2adrifty + chi2brpcy + chi2bdrifty; // event.ychi2;
    fScattering[24] = fMinuitParams[0]; // chi2

    // std::cout << "Doing Double Track Fit" << std::endl;

    // Run an upper and lower single track fit here
    double pocafitparams[14] = {0.};
    pocafit->PerformDoubleTrackPoCAFit(pocafitparams);

    double stf_above_x  = pocafitparams[0];
    double stf_above_px = pocafitparams[1];
    double UPoCA_Vx = fabs(fitVx - (fitVz - stf_above_x) / stf_above_px);

    double stf_above_y  = pocafitparams[2];
    double stf_above_py = pocafitparams[3];
    double UPoCA_Vy = fabs(fitVy - (fitVz - stf_above_y) / stf_above_py);

    double stf_below_x  = pocafitparams[4];
    double stf_below_px = pocafitparams[5];
    double LPoCA_Vx = fabs(fitVx - (fitVz - stf_below_x) / stf_below_px);

    double stf_below_y  = pocafitparams[6];
    double stf_below_py = pocafitparams[7];
    double LPoCA_Vy = fabs(fitVy - (fitVz - stf_below_y) / stf_below_py);

    // PoCA of upper/lower tracks to CF vertex.
    fScattering[25] = UPoCA_Vx;
    fScattering[26] = LPoCA_Vx;
    fScattering[27] = UPoCA_Vy;
    fScattering[28] = LPoCA_Vy;

    // // Save PoCA Results
    fPOCAScattering[0] = pocafitparams[8]; // ScatterX
    fPOCAScattering[1] = pocafitparams[9]; // ScatterY
    fPOCAScattering[2] = pocafitparams[10]; // Scatter3D

    fPOCAScattering[3] = pocafitparams[11]; // fitVx;
    fPOCAScattering[4] = pocafitparams[12]; // fitVy;
    fPOCAScattering[5] = pocafitparams[13]; // fitVz;

    // std::cout << "POCA X : " << fitVx << "  " << pocafitparams[11] << std::endl;
    // std::cout << "POCA Y : " << fitVy << "  " << pocafitparams[12] << std::endl;
    // std::cout << "POCA Z : " << fitVz << "  " << pocafitparams[13] << std::endl;
// 

    // Apply Cuts to fits
    double ChiSquareCut3Points = 1000;
    if (fScattering[18] > ChiSquareCut3Points) continue;
    if (fScattering[19] > ChiSquareCut3Points) continue;
    if (fScattering[20] > ChiSquareCut3Points) continue;
    if (fScattering[21] > ChiSquareCut3Points) continue;

    double CombinedChi2Cut = 1000;
    if (fMinuitParams[0] > CombinedChi2Cut) continue;

    // Fill this event
    otree->Fill();

    // Clean up look sharp
    delete min;
  }

  // Save outputs to TTree
  o->cd();
  otree->Write();
  o->Close();

  return 0;
}




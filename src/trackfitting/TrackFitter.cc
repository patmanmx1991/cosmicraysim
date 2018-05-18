#include "TrackFitter.hh"
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

namespace COSMIC {

BristolSingleTrackFitter::BristolSingleTrackFitter() : fUseRPC(1),
	fUseDrift(1), fMinRPCX(2),
	fMinRPCY(2), fMinDriftX(3), fMinDriftY(3) {
}

BristolSingleTrackFitter::~BristolSingleTrackFitter() {
}

void BristolSingleTrackFitter::DeleteContainers() {

	if (system_rpc_xx) delete system_rpc_xx;
	if (system_rpc_xt) delete system_rpc_xt;
	if (system_rpc_xz) delete system_rpc_xz;
	if (system_rpc_xe) delete system_rpc_xe;

	if (system_rpc_yy) delete system_rpc_yy;
	if (system_rpc_yt) delete system_rpc_yt;
	if (system_rpc_yz) delete system_rpc_yz;
	if (system_rpc_ye) delete system_rpc_ye;

	if (system_drift_xx) delete system_drift_xx;
	if (system_drift_xg) delete system_drift_xg;
	if (system_drift_xt) delete system_drift_xt;
	if (system_drift_xz) delete system_drift_xz;
	if (system_drift_xe) delete system_drift_xe;

	if (system_drift_yy) delete system_drift_yy;
	if (system_drift_yg) delete system_drift_yg;
	if (system_drift_yt) delete system_drift_yt;
	if (system_drift_yz) delete system_drift_yz;
	if (system_drift_ye) delete system_drift_ye;

}

void BristolSingleTrackFitter::ReadInputTTree(TTree* t, std::string prefix) {

	DeleteContainers();

	system_rpc_xx = new std::vector<double>(1, 0.0);
	system_rpc_xt = new std::vector<double>(1, 0.0);
	system_rpc_xz = new std::vector<double>(1, 0.0);
	system_rpc_xe = new std::vector<double>(1, 0.0);
	system_rpc_yy = new std::vector<double>(1, 0.0);
	system_rpc_yt = new std::vector<double>(1, 0.0);
	system_rpc_yz = new std::vector<double>(1, 0.0);
	system_rpc_ye = new std::vector<double>(1, 0.0);
	t->SetBranchAddress( (prefix + "_rpc_xx").c_str(), &system_rpc_xx);
	t->SetBranchAddress( (prefix + "_rpc_xt").c_str(), &system_rpc_xt);
	t->SetBranchAddress( (prefix + "_rpc_xz").c_str(), &system_rpc_xz);
	t->SetBranchAddress( (prefix + "_rpc_xe").c_str(), &system_rpc_xe);
	t->SetBranchAddress( (prefix + "_rpc_yy").c_str(), &system_rpc_yy);
	t->SetBranchAddress( (prefix + "_rpc_yt").c_str(), &system_rpc_yt);
	t->SetBranchAddress( (prefix + "_rpc_yz").c_str(), &system_rpc_yz);
	t->SetBranchAddress( (prefix + "_rpc_ye").c_str(), &system_rpc_ye);

	system_drift_xx = new std::vector<double>(1, 0.0);
	system_drift_xg = new std::vector<double>(1, 0.0);
	system_drift_xt = new std::vector<double>(1, 0.0);
	system_drift_xz = new std::vector<double>(1, 0.0);
	system_drift_xe = new std::vector<double>(1, 0.0);
	system_drift_yy = new std::vector<double>(1, 0.0);
	system_drift_yg = new std::vector<double>(1, 0.0);
	system_drift_yt = new std::vector<double>(1, 0.0);
	system_drift_yz = new std::vector<double>(1, 0.0);
	system_drift_ye = new std::vector<double>(1, 0.0);
	t->SetBranchAddress( (prefix + "_drift_xx").c_str(), &system_drift_xx);
	t->SetBranchAddress( (prefix + "_drift_xg").c_str(), &system_drift_xg);
	t->SetBranchAddress( (prefix + "_drift_xt").c_str(), &system_drift_xt);
	t->SetBranchAddress( (prefix + "_drift_xz").c_str(), &system_drift_xz);
	t->SetBranchAddress( (prefix + "_drift_xe").c_str(), &system_drift_xe);
	t->SetBranchAddress( (prefix + "_drift_yy").c_str(), &system_drift_yy);
	t->SetBranchAddress( (prefix + "_drift_yg").c_str(), &system_drift_yg);
	t->SetBranchAddress( (prefix + "_drift_yt").c_str(), &system_drift_yt);
	t->SetBranchAddress( (prefix + "_drift_yz").c_str(), &system_drift_yz);
	t->SetBranchAddress( (prefix + "_drift_ye").c_str(), &system_drift_ye);

}

void BristolSingleTrackFitter::AddToOutputTTree(TTree* t, std::string prefix) {

	std::string bestfitparslab = prefix + "_trackfit";
	std::string bestfitparsval = "x/D:y:z:px:py";
	t->Branch(bestfitparslab.c_str(), fBestFitPars, bestfitparsval.c_str() );

	std::string minuitparslab = prefix + "_minuitparams";
	std::string minuitparsval = "fmin/D:edm:errdef:nvpar:nparx:istat";
	t->Branch(minuitparslab.c_str(), fMinuitParams, minuitparsval.c_str());

	std::string covariancelab = prefix + "_covar";
	std::string covarianceval = "";
	covarianceval += "c00/D:c01:c02:c03:c04:";
	covarianceval += "c10:c11:c12:c13:c14:";
	covarianceval += "c20:c21:c22:c23:c24:";
	covarianceval += "c30:c31:c32:c33:c34:";
	covarianceval += "c40:c41:c42:c43:c44";
	t->Branch(covariancelab.c_str(), fCovarMatrix, covarianceval.c_str());

}

int BristolSingleTrackFitter::FitTracks() {

	std::cout << "Trying to fit tracks" << std::endl;

	// Return error if not using RPC or DRIFT
	if (!fUseRPC and !fUseDrift) {
		std::cout << "Can't fit without RPC or DRIFTS" << std::endl;
		throw;
	}

	if (system_rpc_xx->size() < fMinRPCX) return 1;
	if (system_rpc_yy->size() < fMinRPCY) return 2;

	if (system_drift_xx->size() < fMinDriftX) return 3;
	if (system_drift_yy->size() < fMinDriftY) return 4;


	// Get the starting values to be used
	int npars = 5;
	double startx = 0.0;
	double starty = 0.0;
	double startz = 0.0;
	double startpx = 0.0;
	double startpy = 0.0;

	// If using RPCs start at top RPC position
	if (fUseRPC) {
		startx  = system_rpc_xx->at(0);
		starty  = system_rpc_yy->at(0);
		startz  = system_rpc_xz->at(0);
		startpx = system_rpc_xx->at(1) - system_rpc_xx->at(0);
		startpy = system_rpc_yy->at(1) - system_rpc_yy->at(0);
	}

	std::cout << "Making minimizer" << std::endl;
	// Create Minimizer Object
	ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
	min->SetPrintLevel(-1);

	// Setup Functor for the fitter
	BristolSingleTrackFitterFCN* fcn = new BristolSingleTrackFitterFCN(this);
	ROOT::Math::Functor func(*fcn, npars);
	min->SetFunction(func);


	// Add our fit variables position + vectors wrt Z axis
	min->SetVariable(0, "rx", startx, 0.1);
	min->SetVariable(1, "ry", starty, 0.1);
	min->SetVariable(2, "rz", startz, 0.1);
	min->SetVariable(3, "px", startpx, 0.1);
	min->SetVariable(4, "py", startpy, 0.1);


	// If using drift chambers Get all combinations of XY hits and push into map
	if (fUseDrift) {

		std::vector<std::vector<bool> > xcombomap;
		std::vector<std::vector<bool> > ycombomap;

		int n = system_drift_xx->size();
		for (int i = 0; i < (1 << n); i++) {
			std::vector<bool> combo;
			for (int j = 0; j < n; j++) {
				combo.push_back(((i & (1 << j)) ? 1 : 0));
			}
			xcombomap.push_back(combo);
		}

		n = system_drift_yy->size();
		for (int i = 0; i < (1 << n); i++) {
			std::vector<bool>combo;
			for (int j = 0; j < n; j++) {
				combo.push_back(((i & (1 << j)) ? 1 : 0));
			}
			ycombomap.push_back(combo);
		}

		// Now loop over all drift combinations if using drifts
		double bestchi2 = -1;

		// Get combo map
		for (uint i = 0; i < xcombomap.size(); i++) {
			for (uint j = 0; j < ycombomap.size(); j++) {

				if (j < i) continue;

				system_drift_xc = &xcombomap.at(i);
				system_drift_yc = &ycombomap.at(j);

				// Run new fit
				min->SetVariable(0, "rx", startx, 0.1);
				min->SetVariable(1, "ry", starty, 0.1);
				min->SetVariable(2, "rz", startz, 0.1);
				min->SetVariable(3, "px", startpx, 0.1);
				min->SetVariable(4, "py", startpy, 0.1);
				min->Minimize();

				// Check if best fit so far
				const double* xxf = min->X();
				double chi2 = DoEval(xxf);

				if (chi2 < bestchi2 || bestchi2 < 0) {

					bestchi2 = chi2;
					min->GetCovMatrix(fCovarMatrix);

					for (int k = 0; k < npars; k++) {
						fBestFitPars[k] = xxf[k];
					}

					Double_t istat = 0;
					if (min->CovMatrixStatus() > 0) istat = 3;

					fMinuitParams[0] = DoEval(fBestFitPars);
					fMinuitParams[1] = min->Edm();
					fMinuitParams[2] = min->ErrorDef();
					fMinuitParams[3] = 7;
					fMinuitParams[4] = 7;
					fMinuitParams[5] = istat;

				}
			}
		}
		xcombomap.clear();
		ycombomap.clear();

		// If not using drift chambers just minimise with RPC hits
	} else {

		min->Minimize();
		const double* xxf = min->X();
		min->GetCovMatrix(fCovarMatrix);

		for (int k = 0; k < npars; k++) {
			fBestFitPars[k] = xxf[k];
		}

		Double_t istat = 0;
		if (min->CovMatrixStatus() > 0) istat = 3;

		fMinuitParams[0] = DoEval(fBestFitPars);
		fMinuitParams[1] = min->Edm();
		fMinuitParams[2] = min->ErrorDef();
		fMinuitParams[3] = 7;
		fMinuitParams[4] = 7;
		fMinuitParams[5] = istat;

	}

	return 0;
}



double BristolSingleTrackFitter::DoEval(const double* x) const {

	double chi2 = 0.0;

	double pointx = x[0];
	double pointy = x[1];
	double pointz = x[2];
	double momx   = x[3];
	double momy   = x[4];
	double momz   = 1.0;

	// Loop over RPCs and get the residual
	if (fUseRPC) {
		for (uint i = 0; i < system_rpc_xx->size(); i++) {
			chi2 += pow( ( system_rpc_xx->at(i) - (pointx + momx * (pointz - system_rpc_xz->at(i)) / momz) ) / system_rpc_xe->at(i), 2 );
		}

		for (uint i = 0; i < system_rpc_yy->size(); i++) {
			chi2 += pow( ( system_rpc_yy->at(i) - (pointy + momy * (pointz - system_rpc_yz->at(i)) / momz) ) / system_rpc_ye->at(i), 2 );
		}
	}

	// Loop over bristol SD and get the residual.
	// Only use chosen combinations
	if (fUseDrift) {
		for (uint i = 0; i < system_drift_xx->size(); i++) {
			if (system_drift_xc->at(i)) {
				chi2 += pow( ( system_drift_xg->at(i) - (pointx + momx * (pointz - system_drift_xz->at(i)) / momz) ) / system_drift_xe->at(i), 2 );
			} else {
				chi2 += pow( ( system_drift_xx->at(i) - (pointx + momx * (pointz - system_drift_xz->at(i)) / momz) ) / system_drift_xe->at(i), 2 );
			}
		}

		for (uint i = 0; i < system_drift_yy->size(); i++) {
			if (system_drift_yc->at(i)) {
				chi2 += pow( ( system_drift_yg->at(i) - (pointy + momy * (pointz - system_drift_yz->at(i)) / momz) ) / system_drift_ye->at(i), 2 );
			} else {
				chi2 += pow( ( system_drift_yy->at(i) - (pointy + momy * (pointz - system_drift_yz->at(i)) / momz) ) / system_drift_ye->at(i), 2 );
			}
		}
	}

	return chi2;
}

}










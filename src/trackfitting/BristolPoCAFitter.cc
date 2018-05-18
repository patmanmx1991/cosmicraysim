#include "BristolPoCAFitter.hh"
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

BristolPoCAFitter::BristolPoCAFitter() : fUseRPC(1),
	fUseDrift(1), 
	fMinARPCX(2),
	fMinARPCY(2),
	fMinBRPCX(2),
	fMinBRPCY(2),  
	fMinADriftX(3), 
	fMinADriftY(3),
	fMinBDriftX(3), 
	fMinBDriftY(3) {
}

BristolPoCAFitter::~BristolPoCAFitter() {
}

void BristolPoCAFitter::DeleteContainers() {

	if (above_rpc_xx) delete above_rpc_xx;
	if (above_rpc_xt) delete above_rpc_xt;
	if (above_rpc_xz) delete above_rpc_xz;
	if (above_rpc_xe) delete above_rpc_xe;

	if (above_rpc_yy) delete above_rpc_yy;
	if (above_rpc_yt) delete above_rpc_yt;
	if (above_rpc_yz) delete above_rpc_yz;
	if (above_rpc_ye) delete above_rpc_ye;

	if (above_drift_xx) delete above_drift_xx;
	if (above_drift_xg) delete above_drift_xg;
	if (above_drift_xt) delete above_drift_xt;
	if (above_drift_xz) delete above_drift_xz;
	if (above_drift_xe) delete above_drift_xe;

	if (above_drift_yy) delete above_drift_yy;
	if (above_drift_yg) delete above_drift_yg;
	if (above_drift_yt) delete above_drift_yt;
	if (above_drift_yz) delete above_drift_yz;
	if (above_drift_ye) delete above_drift_ye;

	if (below_rpc_xx) delete below_rpc_xx;
	if (below_rpc_xt) delete below_rpc_xt;
	if (below_rpc_xz) delete below_rpc_xz;
	if (below_rpc_xe) delete below_rpc_xe;

	if (below_rpc_yy) delete below_rpc_yy;
	if (below_rpc_yt) delete below_rpc_yt;
	if (below_rpc_yz) delete below_rpc_yz;
	if (below_rpc_ye) delete below_rpc_ye;

	if (below_drift_xx) delete below_drift_xx;
	if (below_drift_xg) delete below_drift_xg;
	if (below_drift_xt) delete below_drift_xt;
	if (below_drift_xz) delete below_drift_xz;
	if (below_drift_xe) delete below_drift_xe;

	if (below_drift_yy) delete below_drift_yy;
	if (below_drift_yg) delete below_drift_yg;
	if (below_drift_yt) delete below_drift_yt;
	if (below_drift_yz) delete below_drift_yz;
	if (below_drift_ye) delete below_drift_ye;

}

void BristolPoCAFitter::ReadInputTTree(TTree* t, std::string prefixa, std::string prefixb) {

	DeleteContainers();

	above_rpc_xx = new std::vector<double>(1, 0.0);
	above_rpc_xt = new std::vector<double>(1, 0.0);
	above_rpc_xz = new std::vector<double>(1, 0.0);
	above_rpc_xe = new std::vector<double>(1, 0.0);
	above_rpc_yy = new std::vector<double>(1, 0.0);
	above_rpc_yt = new std::vector<double>(1, 0.0);
	above_rpc_yz = new std::vector<double>(1, 0.0);
	above_rpc_ye = new std::vector<double>(1, 0.0);
	t->SetBranchAddress( (prefixa + "_rpc_xx").c_str(), &above_rpc_xx);
	t->SetBranchAddress( (prefixa + "_rpc_xt").c_str(), &above_rpc_xt);
	t->SetBranchAddress( (prefixa + "_rpc_xz").c_str(), &above_rpc_xz);
	t->SetBranchAddress( (prefixa + "_rpc_xe").c_str(), &above_rpc_xe);
	t->SetBranchAddress( (prefixa + "_rpc_yy").c_str(), &above_rpc_yy);
	t->SetBranchAddress( (prefixa + "_rpc_yt").c_str(), &above_rpc_yt);
	t->SetBranchAddress( (prefixa + "_rpc_yz").c_str(), &above_rpc_yz);
	t->SetBranchAddress( (prefixa + "_rpc_ye").c_str(), &above_rpc_ye);

	above_drift_xx = new std::vector<double>(1, 0.0);
	above_drift_xg = new std::vector<double>(1, 0.0);
	above_drift_xt = new std::vector<double>(1, 0.0);
	above_drift_xz = new std::vector<double>(1, 0.0);
	above_drift_xe = new std::vector<double>(1, 0.0);
	above_drift_yy = new std::vector<double>(1, 0.0);
	above_drift_yg = new std::vector<double>(1, 0.0);
	above_drift_yt = new std::vector<double>(1, 0.0);
	above_drift_yz = new std::vector<double>(1, 0.0);
	above_drift_ye = new std::vector<double>(1, 0.0);
	t->SetBranchAddress( (prefixa + "_drift_xx").c_str(), &above_drift_xx);
	t->SetBranchAddress( (prefixa + "_drift_xg").c_str(), &above_drift_xg);
	t->SetBranchAddress( (prefixa + "_drift_xt").c_str(), &above_drift_xt);
	t->SetBranchAddress( (prefixa + "_drift_xz").c_str(), &above_drift_xz);
	t->SetBranchAddress( (prefixa + "_drift_xe").c_str(), &above_drift_xe);
	t->SetBranchAddress( (prefixa + "_drift_yy").c_str(), &above_drift_yy);
	t->SetBranchAddress( (prefixa + "_drift_yg").c_str(), &above_drift_yg);
	t->SetBranchAddress( (prefixa + "_drift_yt").c_str(), &above_drift_yt);
	t->SetBranchAddress( (prefixa + "_drift_yz").c_str(), &above_drift_yz);
	t->SetBranchAddress( (prefixa + "_drift_ye").c_str(), &above_drift_ye);

	below_rpc_xx = new std::vector<double>(1, 0.0);
	below_rpc_xt = new std::vector<double>(1, 0.0);
	below_rpc_xz = new std::vector<double>(1, 0.0);
	below_rpc_xe = new std::vector<double>(1, 0.0);
	below_rpc_yy = new std::vector<double>(1, 0.0);
	below_rpc_yt = new std::vector<double>(1, 0.0);
	below_rpc_yz = new std::vector<double>(1, 0.0);
	below_rpc_ye = new std::vector<double>(1, 0.0);
	t->SetBranchAddress( (prefixb + "_rpc_xx").c_str(), &below_rpc_xx);
	t->SetBranchAddress( (prefixb + "_rpc_xt").c_str(), &below_rpc_xt);
	t->SetBranchAddress( (prefixb + "_rpc_xz").c_str(), &below_rpc_xz);
	t->SetBranchAddress( (prefixb + "_rpc_xe").c_str(), &below_rpc_xe);
	t->SetBranchAddress( (prefixb + "_rpc_yy").c_str(), &below_rpc_yy);
	t->SetBranchAddress( (prefixb + "_rpc_yt").c_str(), &below_rpc_yt);
	t->SetBranchAddress( (prefixb + "_rpc_yz").c_str(), &below_rpc_yz);
	t->SetBranchAddress( (prefixb + "_rpc_ye").c_str(), &below_rpc_ye);

	below_drift_xx = new std::vector<double>(1, 0.0);
	below_drift_xg = new std::vector<double>(1, 0.0);
	below_drift_xt = new std::vector<double>(1, 0.0);
	below_drift_xz = new std::vector<double>(1, 0.0);
	below_drift_xe = new std::vector<double>(1, 0.0);
	below_drift_yy = new std::vector<double>(1, 0.0);
	below_drift_yg = new std::vector<double>(1, 0.0);
	below_drift_yt = new std::vector<double>(1, 0.0);
	below_drift_yz = new std::vector<double>(1, 0.0);
	below_drift_ye = new std::vector<double>(1, 0.0);
	t->SetBranchAddress( (prefixb + "_drift_xx").c_str(), &below_drift_xx);
	t->SetBranchAddress( (prefixb + "_drift_xg").c_str(), &below_drift_xg);
	t->SetBranchAddress( (prefixb + "_drift_xt").c_str(), &below_drift_xt);
	t->SetBranchAddress( (prefixb + "_drift_xz").c_str(), &below_drift_xz);
	t->SetBranchAddress( (prefixb + "_drift_xe").c_str(), &below_drift_xe);
	t->SetBranchAddress( (prefixb + "_drift_yy").c_str(), &below_drift_yy);
	t->SetBranchAddress( (prefixb + "_drift_yg").c_str(), &below_drift_yg);
	t->SetBranchAddress( (prefixb + "_drift_yt").c_str(), &below_drift_yt);
	t->SetBranchAddress( (prefixb + "_drift_yz").c_str(), &below_drift_yz);
	t->SetBranchAddress( (prefixb + "_drift_ye").c_str(), &below_drift_ye);

}

void BristolPoCAFitter::AddToOutputTTree(TTree* t, std::string prefix) {

	std::string bestfitparslab = prefix + "_trackfit";
	std::string bestfitparsval = "x/D:y:z:px1:py1:px2:py2";
	t->Branch(bestfitparslab.c_str(), fBestFitPars, bestfitparsval.c_str() );

	std::string minuitparslab = prefix + "_minuitparams";
	std::string minuitparsval = "fmin/D:edm:errdef:nvpar:nparx:istat";
	t->Branch(minuitparslab.c_str(), fMinuitParams, minuitparsval.c_str());

	std::string covariancelab = prefix + "_covar";
	std::string covarianceval = "";
	covarianceval += "c00/D:c01:c02:c03:c04:c05:c06:";
	covarianceval += "c10:c11:c12:c13:c14:c15:c16:";
	covarianceval += "c20:c21:c22:c23:c24:c25:c26:";
	covarianceval += "c30:c31:c32:c33:c34:c35:c36:";
	covarianceval += "c40:c41:c42:c43:c44:c45:c46:";
	covarianceval += "c50:c51:c52:c53:c54:c55:c56:";
	covarianceval += "c60:c61:c62:c63:c64:c65:c66";
	t->Branch(covariancelab.c_str(), fCovarMatrix, covarianceval.c_str());

}

int BristolPoCAFitter::FitTracks() {

	std::cout << "Trying to fit tracks" << std::endl;

	// Return error if not using RPC or DRIFT
	if (!fUseRPC and !fUseDrift) {
		std::cout << "Can't fit without RPC or DRIFTS" << std::endl;
		throw;
	}

	if (above_rpc_xx->size() < fMinARPCX) return 1;
	if (above_rpc_yy->size() < fMinARPCY) return 2;
	if (above_drift_xx->size() < fMinADriftX) return 3;
	if (above_drift_yy->size() < fMinADriftY) return 4;
	if (below_rpc_xx->size() < fMinBRPCX) return 5;
	if (below_rpc_yy->size() < fMinBRPCY) return 6;
	if (below_drift_xx->size() < fMinBDriftX) return 7;
	if (below_drift_yy->size() < fMinBDriftY) return 8;

	// Get the starting values to be used
	int npars = 7;
	double startx = 0.0;
	double starty = 0.0;
	double startz = 0.0;
	double startpx1 = 0.0;
	double startpy1 = 0.0;
	double startpx2 = 0.0;
	double startpy2 = 0.0;

	// If using RPCs start at top RPC position
	if (fUseRPC) {
		startx  = above_rpc_xx->at(0);
		starty  = above_rpc_yy->at(0);
		startz  = above_rpc_xz->at(0);
		startpx1 = above_rpc_xx->at(1) - above_rpc_xx->at(0);
		startpy1 = above_rpc_yy->at(1) - above_rpc_yy->at(0);
		startpx2 = below_rpc_xx->at(1) - below_rpc_xx->at(0);
		startpy2 = below_rpc_yy->at(1) - below_rpc_yy->at(0);
	}

	std::cout << "Making minimizer" << std::endl;
	// Create Minimizer Object
	ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
	min->SetPrintLevel(-1);

	// Setup Functor for the fitter
	BristolPoCAFitterFCN* fcn = new BristolPoCAFitterFCN(this);
	ROOT::Math::Functor func(*fcn, npars);
	min->SetFunction(func);

	// If using drift chambers Get all combinations of XY hits and push into map
	if (fUseDrift) {

		std::vector<std::vector<bool> > xcombomapa;
		std::vector<std::vector<bool> > ycombomapa;
		std::vector<std::vector<bool> > xcombomapb;
		std::vector<std::vector<bool> > ycombomapb;

		int n = above_drift_xx->size();
		for (int i = 0; i < (1 << n); i++) {
			std::vector<bool> combo;
			for (int j = 0; j < n; j++) {
				combo.push_back(((i & (1 << j)) ? 1 : 0));
			}
			xcombomapa.push_back(combo);
		}

		n = above_drift_yy->size();
		for (int i = 0; i < (1 << n); i++) {
			std::vector<bool>combo;
			for (int j = 0; j < n; j++) {
				combo.push_back(((i & (1 << j)) ? 1 : 0));
			}
			ycombomapa.push_back(combo);
		}

		n = below_drift_xx->size();
		for (int i = 0; i < (1 << n); i++) {
			std::vector<bool> combo;
			for (int j = 0; j < n; j++) {
				combo.push_back(((i & (1 << j)) ? 1 : 0));
			}
			xcombomapb.push_back(combo);
		}

		n = below_drift_yy->size();
		for (int i = 0; i < (1 << n); i++) {
			std::vector<bool>combo;
			for (int j = 0; j < n; j++) {
				combo.push_back(((i & (1 << j)) ? 1 : 0));
			}
			ycombomapb.push_back(combo);
		}

		// Now loop over all drift combinations if using drifts
		double bestchi2 = -1;


		// Get combo map
		for (uint i = 0; i < xcombomapa.size(); i++) {
			for (uint j = 0; j < ycombomapa.size(); j++) {
				for (uint g = 0; g < xcombomapb.size(); g++) {
					for (uint h = 0; h < ycombomapb.size(); h++) {

						std::cout << " TRYING NEW FIT " << i << j << g << h << " MEV = " << MeV << std::endl;

						std::cout << 2.65 << " vs " << 2.65 * g/cm3 << std::endl;
						above_drift_xc = &xcombomapa.at(i);
						above_drift_yc = &ycombomapa.at(j);
						below_drift_xc = &xcombomapb.at(g);
						below_drift_yc = &ycombomapb.at(h);

						// Run new fit
						min->SetVariable(0, "rx", startx, 0.1);
						min->SetVariable(1, "ry", starty, 0.1);
						min->SetVariable(2, "rz", startz, 0.1);
						min->SetVariable(3, "px1", startpx1, 0.1);
						min->SetVariable(4, "py1", startpy1, 0.1);
						min->SetVariable(5, "px2", startpx2, 0.1);
						min->SetVariable(6, "py2", startpy2, 0.1);
						min->Minimize();

						// Check if best fit so far
						const double* xxf = min->X();
						double chi2 = DoEval(xxf);
						std::cout << " CHI2 : " << chi2 << std::endl;

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
			}
		}
		xcombomapa.clear();
		ycombomapa.clear();
		xcombomapb.clear();
		xcombomapb.clear();

		// If not using drift chambers just minimise with RPC hits
	} else {

		min->SetVariable(0, "rx", startx, 0.1);
		min->SetVariable(1, "ry", starty, 0.1);
		min->SetVariable(2, "rz", startz, 0.1);
		min->SetVariable(3, "px1", startpx1, 0.1);
		min->SetVariable(4, "py1", startpy1, 0.1);
		min->SetVariable(5, "px2", startpx2, 0.1);
		min->SetVariable(6, "py2", startpy2, 0.1);
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


	std::cout << " Found Good Track Fit : " << DoEval(fBestFitPars) << std::endl;
	sleep(10);
	return 0;
}



double BristolPoCAFitter::DoEval(const double* x) const {

	double chi2 = 0.0;

	double pointx = x[0];
	double pointy = x[1];
	double pointz = x[2];
	double momx1   = x[3];
	double momy1   = x[4];
	double momx2   = x[5];
	double momy2   = x[6];
	double momz   = 1.0;

	// Loop over RPCs and get the residual
	if (fUseRPC) {
		for (uint i = 0; i < above_rpc_xx->size(); i++) {
			chi2 += pow( ( above_rpc_xx->at(i) - (pointx + momx1 * (pointz - above_rpc_xz->at(i)) / momz) ) / above_rpc_xe->at(i), 2 );
		}

		for (uint i = 0; i < above_rpc_yy->size(); i++) {
			chi2 += pow( ( above_rpc_yy->at(i) - (pointy + momy1 * (pointz - above_rpc_yz->at(i)) / momz) ) / above_rpc_ye->at(i), 2 );
		}

		for (uint i = 0; i < below_rpc_xx->size(); i++) {
			chi2 += pow( ( below_rpc_xx->at(i) - (pointx + momx2 * (pointz - below_rpc_xz->at(i)) / momz) ) / below_rpc_xe->at(i), 2 );
		}

		for (uint i = 0; i < below_rpc_yy->size(); i++) {
			chi2 += pow( ( below_rpc_yy->at(i) - (pointy + momy2 * (pointz - below_rpc_yz->at(i)) / momz) ) / below_rpc_ye->at(i), 2 );
		}

	}

	// Loop over bristol SD and get the residual.
	// Only use chosen combinations
	if (fUseDrift) {
		for (uint i = 0; i < above_drift_xx->size(); i++) {
			if (above_drift_xc->at(i)) {
				chi2 += pow( ( above_drift_xg->at(i) - (pointx + momx1 * (pointz - above_drift_xz->at(i)) / momz) ) / above_drift_xe->at(i), 2 );
			} else {
				chi2 += pow( ( above_drift_xx->at(i) - (pointx + momx1 * (pointz - above_drift_xz->at(i)) / momz) ) / above_drift_xe->at(i), 2 );
			}
		}

		for (uint i = 0; i < above_drift_yy->size(); i++) {
			if (above_drift_yc->at(i)) {
				chi2 += pow( ( above_drift_yg->at(i) - (pointy + momy1 * (pointz - above_drift_yz->at(i)) / momz) ) / above_drift_ye->at(i), 2 );
			} else {
				chi2 += pow( ( above_drift_yy->at(i) - (pointy + momy1 * (pointz - above_drift_yz->at(i)) / momz) ) / above_drift_ye->at(i), 2 );
			}
		}

		for (uint i = 0; i < below_drift_xx->size(); i++) {
			if (below_drift_xc->at(i)) {
				chi2 += pow( ( below_drift_xg->at(i) - (pointx + momx2 * (pointz - below_drift_xz->at(i)) / momz) ) / below_drift_xe->at(i), 2 );
			} else {
				chi2 += pow( ( below_drift_xx->at(i) - (pointx + momx2 * (pointz - below_drift_xz->at(i)) / momz) ) / below_drift_xe->at(i), 2 );
			}
		}

		for (uint i = 0; i < below_drift_yy->size(); i++) {
			if (below_drift_yc->at(i)) {
				chi2 += pow( ( below_drift_yg->at(i) - (pointy + momy2 * (pointz - below_drift_yz->at(i)) / momz) ) / below_drift_ye->at(i), 2 );
			} else {
				chi2 += pow( ( below_drift_yy->at(i) - (pointy + momy2 * (pointz - below_drift_yz->at(i)) / momz) ) / below_drift_ye->at(i), 2 );
			}
		}
	}

	return chi2;
}

}










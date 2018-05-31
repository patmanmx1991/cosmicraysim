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
#include "TVector3.h"
#include "Minuit2/MnPrint.h"

namespace COSMIC {

// ------------------------------------------------------------------------
// Constructor/ IO
// ------------------------------------------------------------------------

BristolPoCAFitter::BristolPoCAFitter() :
	fUseRPC(1),
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

void BristolPoCAFitter::ReadInputTTree(TTree* t, std::string prefixa, std::string prefixb) {

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


// ------------------------------------------------------------------------
// Main Fit Evaluator
// ------------------------------------------------------------------------

double BristolPoCAFitter::DoEval(const double* x) const {

	double chi2 = 0.0;

	// std::cout << "Doing Eval "
	//           << x[0] << " "
	//           << x[1] << " "
	//           << x[2] << " "
	//           << x[3] << " "
	//           << x[4] << " "
	//           << x[5] << " "
	//           << x[6] << " " << std::endl;

	// Include all RPC and drift hits
	double pointx = x[0];
	double pointy = x[1];
	double pointz = x[2];
	double momx1  = x[3];
	double momx2  = x[4];
	double momy1  = x[5];
	double momy2  = x[6];

	// std::cout << "Getting RPC Hits" << std::endl;
	chi2 += GetChi2AboveRPCX( pointx, momx2, pointz );
	chi2 += GetChi2AboveRPCY( pointy, momy2, pointz );
	chi2 += GetChi2BelowRPCX( pointx, momx1, pointz );
	chi2 += GetChi2BelowRPCY( pointy, momy1, pointz );

	chi2 += GetChi2AboveDriftX( pointx, momx2, pointz );
	chi2 += GetChi2AboveDriftY( pointy, momy2, pointz );
	chi2 += GetChi2BelowDriftX( pointx, momx1, pointz );
	chi2 += GetChi2BelowDriftY( pointy, momy1, pointz );

	// std::cout << "Print Level : " << ROOT::Minuit2::MnPrint::Level() << std::endl;

	// std::cout << " Total Chi2 = " << chi2 << std::endl;
	return chi2;

}

double BristolPoCAFitter::GetChi2AboveRPCX( double pointx, double momx1, double pointz ) const {

	if (!fUseARPCX) return 0.0;

	double chi2 = 0.0;
	for (uint i = 0; i < above_rpc_xx->size(); i++) {
		chi2 += pow( ( above_rpc_xx->at(i) - (pointx + momx1 * (pointz - above_rpc_xz->at(i))) ) / above_rpc_xe->at(i), 2 );
	}

	return chi2;
}

double BristolPoCAFitter::GetChi2AboveRPCY( double pointy, double momy1, double pointz ) const {

	if (!fUseARPCY) return 0.0;

	double chi2 = 0.0;
	for (uint i = 0; i < above_rpc_yy->size(); i++) {
		chi2 += pow( ( above_rpc_yy->at(i) - (pointy + momy1 * (pointz - above_rpc_yz->at(i))) ) / above_rpc_ye->at(i), 2 );
	}

	return chi2;
}


double BristolPoCAFitter::GetChi2AboveDriftX( double pointx, double momx1, double pointz ) const {

	if (!fUseADriftX) return 0.0;

	double chi2 = 0.0;

	for (uint i = 0; i < above_drift_xx->size(); i++) {
		if (above_drift_xc->at(i)) {
			chi2 += pow( ( above_drift_xg->at(i) - (pointx + momx1 * (pointz - above_drift_xz->at(i))) ) / above_drift_xe->at(i), 2 );
		} else {
			chi2 += pow( ( above_drift_xx->at(i) - (pointx + momx1 * (pointz - above_drift_xz->at(i))) ) / above_drift_xe->at(i), 2 );
		}
	}

	return chi2;
}


double BristolPoCAFitter::GetChi2AboveDriftY( double pointy, double momy1, double pointz ) const {

	if (!fUseADriftY) return 0.0;

	double chi2 = 0.0;

	for (uint i = 0; i < above_drift_yy->size(); i++) {
		if (above_drift_yc->at(i)) {
			chi2 += pow( ( above_drift_yg->at(i) - (pointy + momy1 * (pointz - above_drift_yz->at(i))) ) / above_drift_ye->at(i), 2 );
		} else {
			chi2 += pow( ( above_drift_yy->at(i) - (pointy + momy1 * (pointz - above_drift_yz->at(i))) ) / above_drift_ye->at(i), 2 );
		}
	}

	return chi2;
}

double BristolPoCAFitter::GetChi2BelowRPCX( double pointx, double momx2, double pointz ) const {

	if (!fUseBRPCX) return 0.0;

	double chi2 = 0.0;
	for (uint i = 0; i < below_rpc_xx->size(); i++) {
		chi2 += pow( ( below_rpc_xx->at(i) - (pointx + momx2 * (pointz - below_rpc_xz->at(i))) ) / below_rpc_xe->at(i), 2 );
	}

	return chi2;
}

double BristolPoCAFitter::GetChi2BelowRPCY( double pointy, double momy2, double pointz ) const {

	if (!fUseBRPCY) return 0.0;

	double chi2 = 0.0;
	for (uint i = 0; i < below_rpc_yy->size(); i++) {
		chi2 += pow( ( below_rpc_yy->at(i) - (pointy + momy2 * (pointz - below_rpc_yz->at(i))) ) / below_rpc_ye->at(i), 2 );
	}

	return chi2;
}


double BristolPoCAFitter::GetChi2BelowDriftX( double pointx, double momx2, double pointz ) const {

	if (!fUseBDriftX) return 0.0;

	double chi2 = 0.0;

	for (uint i = 0; i < below_drift_xx->size(); i++) {
		if (below_drift_xc->at(i)) {
			chi2 += pow( ( below_drift_xg->at(i) - (pointx + momx2 * (pointz - below_drift_xz->at(i))) ) / below_drift_xe->at(i), 2 );
		} else {
			chi2 += pow( ( below_drift_xx->at(i) - (pointx + momx2 * (pointz - below_drift_xz->at(i))) ) / below_drift_xe->at(i), 2 );
		}
	}

	return chi2;
}


double BristolPoCAFitter::GetChi2BelowDriftY( double pointy, double momy2, double pointz ) const {

	if (!fUseBDriftY) return 0.0;

	double chi2 = 0.0;
	for (uint i = 0; i < below_drift_yy->size(); i++) {
		if (below_drift_yc->at(i)) {
			chi2 += pow( ( below_drift_yg->at(i) - (pointy + momy2 * (pointz - below_drift_yz->at(i))) ) / below_drift_ye->at(i), 2 );
		} else {
			chi2 += pow( ( below_drift_yy->at(i) - (pointy + momy2 * (pointz - below_drift_yz->at(i))) ) / below_drift_ye->at(i), 2 );
		}
	}

	return chi2;
}


// ------------------------------------------------------------------------
// Single Container Handling (fit single fits and drift evaluation)
// ------------------------------------------------------------------------
void BristolPoCAFitter::ClearSingleContainers() {

	values_x.clear();
	values_e.clear();
	values_g.clear();
	values_z.clear();

	values_rx.clear();
	values_re.clear();
	values_rz.clear();

}

void BristolPoCAFitter::FillSingleContainers(int drifttype) {

	// Clear the containers first
	ClearSingleContainers();

	// Now fill according to the bools
	SetHitUsage(drifttype);

	// RPC Hits First
	if (fUseARPCX) {
		for (int j = 0; j < above_rpc_xx->size(); j++) {
			values_rx.push_back(above_rpc_xx->at(j));
			values_re.push_back(above_rpc_xe->at(j));
			values_rz.push_back(above_rpc_xz->at(j));
		}
	}

	if (fUseARPCY) {
		for (int j = 0; j < above_rpc_yy->size(); j++) {
			values_rx.push_back(above_rpc_yy->at(j));
			values_re.push_back(above_rpc_ye->at(j));
			values_rz.push_back(above_rpc_yz->at(j));
		}
	}

	if (fUseBRPCX) {
		for (int j = 0; j < below_rpc_xx->size(); j++) {
			values_rx.push_back(below_rpc_xx->at(j));
			values_re.push_back(below_rpc_xe->at(j));
			values_rz.push_back(below_rpc_xz->at(j));
		}
	}

	if (fUseBRPCY) {
		for (int j = 0; j < below_rpc_yy->size(); j++) {
			values_rx.push_back(below_rpc_yy->at(j));
			values_re.push_back(below_rpc_ye->at(j));
			values_rz.push_back(below_rpc_yz->at(j));
		}
	}

	// Now Drift Hits
	if (fUseADriftX) {
		for (int j = 0; j < above_drift_xx->size(); j++) {
			values_x.push_back(above_drift_xx->at(j));
			values_e.push_back(above_drift_xe->at(j));
			values_g.push_back(above_drift_xg->at(j));
			values_z.push_back(above_drift_xz->at(j));
		}
	}

	if (fUseADriftY) {
		for (int j = 0; j < above_drift_yy->size(); j++) {
			values_x.push_back(above_drift_yy->at(j));
			values_e.push_back(above_drift_ye->at(j));
			values_g.push_back(above_drift_yg->at(j));
			values_z.push_back(above_drift_yz->at(j));
		}
	}

	if (fUseBDriftX) {
		for (int j = 0; j < below_drift_xx->size(); j++) {
			values_x.push_back(below_drift_xx->at(j));
			values_e.push_back(below_drift_xe->at(j));
			values_g.push_back(below_drift_xg->at(j));
			values_z.push_back(below_drift_xz->at(j));
		}
	}

	if (fUseBDriftY) {
		for (int j = 0; j < below_drift_yy->size(); j++) {
			values_x.push_back(below_drift_yy->at(j));
			values_e.push_back(below_drift_ye->at(j));
			values_g.push_back(below_drift_yg->at(j));
			values_z.push_back(below_drift_yz->at(j));
		}
	}
}


double BristolPoCAFitter::PredictStartX() {

	double avg = 0.0;
	int n = 0;

	for (uint i = 0; i < values_x.size(); i++) {

		double x = values_x.at(i);
		if (values_c->at(i)) x = values_g.at(i);

		avg += x;
		n++;
	}

	for (uint i = 0; i < values_rx.size(); i++) {
		avg += values_rx.at(i);
		n++;
	}

	return avg / double(n);
}


double BristolPoCAFitter::PredictStartZ() {

	double avg = 0.0;
	int n = 0;

	for (uint i = 0; i < values_z.size(); i++) {
		avg += values_z.at(i);
		n++;
	}

	for (uint i = 0; i < values_rz.size(); i++) {
		avg += values_rz.at(i);
		n++;
	}

	return avg / double(n);
}

double BristolPoCAFitter::PredictStartPX() {

	double minx = -999.;
	double maxx = -999.;
	double minz = -999.;
	double maxz = -999.;

	for (uint i = 0; i < values_x.size(); i++) {

		double x = values_x.at(i);
		if (values_c->at(i)) {
			x = values_g.at(i);
		}

		double z = values_z.at(i);

		if ( minx == -999. || x < minx ) minx = x;
		if ( maxx == -999. || x > minx ) maxx = x;
		if ( minz == -999. || z < minz ) minz = z;
		if ( maxz == -999. || z > minz ) maxz = z;
	}

	for (uint i = 0; i < values_rx.size(); i++) {

		double x = values_rx.at(i);
		double z = values_rz.at(i);

		if ( minx == -999. || x < minx ) minx = x;
		if ( maxx == -999. || x > minx ) maxx = x;
		if ( minz == -999. || z < minz ) minz = z;
		if ( maxz == -999. || z > minz ) maxz = z;
	}

	if (maxx - minx == 0.0) return 1.0;

	return (maxz - minz) / (maxx - minx);
}


double BristolPoCAFitter::GetLowestX() {

	double minx = -999.;
	double minz = -999.;

	for (uint i = 0; i < values_x.size(); i++) {

		double x = values_x.at(i);
		if (values_c->at(i)) {
			x = values_g.at(i);
		}
		double z = values_z.at(i);

		if ( minz == -999. || z < minz ){	
			minx = x;
			minz = z;
		} 
	}

	for (uint i = 0; i < values_rx.size(); i++) {

		double x = values_rx.at(i);
		double z = values_rz.at(i);

		if ( minz == -999. || z < minz ) {
			minx = x;
			minz = z;
		}
	}

	return minx;
}


double BristolPoCAFitter::GetLowestZ() {

	double minx = -999.;
	double minz = -999.;

	for (uint i = 0; i < values_x.size(); i++) {

		double x = values_x.at(i);
		if (values_c->at(i)) {
			x = values_g.at(i);
		}
		double z = values_z.at(i);

		if ( minz == -999. || z < minz ){	
			minx = x;
			minz = z;
		} 
	}

	for (uint i = 0; i < values_rx.size(); i++) {

		double x = values_rx.at(i);
		double z = values_rz.at(i);

		if ( minz == -999. || z < minz ) {
			minx = x;
			minz = z;
			// std::cout << " New Lowest RPC Z " << minz << std::endl;

		}
	}

	// std::cout << "Returning Lowest Z : " << minz << std::endl;
	return minz;
}


double BristolPoCAFitter::GetHighestX() {

	double maxx = -999.;
	double maxz = -999.;

	for (uint i = 0; i < values_x.size(); i++) {

		double x = values_x.at(i);
		if (values_c->at(i)) {
			x = values_g.at(i);
		}
		double z = values_z.at(i);

		if ( maxz == -999. || z > maxz ){	
			maxx = x;
			maxz = z;
		} 
	}

	for (uint i = 0; i < values_rx.size(); i++) {

		double x = values_rx.at(i);
		double z = values_rz.at(i);

		if ( maxz == -999. || z > maxz ) {
			maxx = x;
			maxz = z;
		}
	}

	return maxx;
}


double BristolPoCAFitter::GetHighestZ() {

	double maxx = -999.;
	double maxz = -999.;

	for (uint i = 0; i < values_x.size(); i++) {

		double x = values_x.at(i);
		if (values_c->at(i)) {
			x = values_g.at(i);
		}
		double z = values_z.at(i);

		if ( maxz == -999. || z > maxz ){	
			maxx = x;
			maxz = z;
		} 
	}

	for (uint i = 0; i < values_rx.size(); i++) {

		double x = values_rx.at(i);
		double z = values_rz.at(i);

		if ( maxz == -999. || z > maxz ) {
			maxx = x;
			maxz = z;
		}
	}

	return maxz;
}


double BristolPoCAFitter::PredictStartAbovePX() {
	FillSingleContainers(kFitAllAboveX);
	SetVectorC( above_drift_xc );
	return PredictStartPX();
}

double BristolPoCAFitter::PredictStartAbovePY() {
	FillSingleContainers(kFitAllAboveY);
	SetVectorC( above_drift_yc );
	return PredictStartPX();
}

double BristolPoCAFitter::PredictStartBelowPX() {
	FillSingleContainers(kFitAllBelowX);
	SetVectorC( below_drift_xc );

	return PredictStartPX();
}

double BristolPoCAFitter::PredictStartBelowPY() {
	FillSingleContainers(kFitAllBelowY);
	SetVectorC( below_drift_yc );
	return PredictStartPX();
}

double BristolPoCAFitter::PredictStartPoCAX() {
	FillSingleContainers(kFitAllAboveX);
	SetVectorC( above_drift_xc );
	double xa = PredictStartX();
	FillSingleContainers(kFitAllBelowX);
	SetVectorC( below_drift_xc );
	double xb = PredictStartX();
	return (xa + xb) / 2;
}

double BristolPoCAFitter::PredictStartPoCAY() {
	FillSingleContainers(kFitAllAboveY);
	SetVectorC( above_drift_yc );
	double ya = PredictStartX();
	FillSingleContainers(kFitAllBelowY);
	SetVectorC( below_drift_yc );
	double yb = PredictStartX();
	return (ya + yb) / 2;
}

double BristolPoCAFitter::PredictStartPoCAZ() {
	FillSingleContainers(kFitAllAboveX);
	SetVectorC( above_drift_xc );
	double zxa = PredictStartZ();
	FillSingleContainers(kFitAllBelowX);
	SetVectorC( below_drift_xc );
	double zxb = PredictStartZ();
	FillSingleContainers(kFitAllAboveY);
	SetVectorC( above_drift_yc );
	double zya = PredictStartZ();
	FillSingleContainers(kFitAllBelowY);
	SetVectorC( below_drift_yc );
	double zyb = PredictStartZ();
	return (zxa + zxb + zya + zyb) / 4;
}


// ------------------------------------------------------------------------
// Evaluation Flags
// ------------------------------------------------------------------------
void BristolPoCAFitter::SetHitUsage(int drifttype) {

	switch (drifttype) {

	case kFitDriftAboveX: SetAboveDriftXOnly(); break;
	case kFitDriftAboveY: SetAboveDriftYOnly(); break;
	case kFitDriftBelowX: SetBelowDriftXOnly(); break;
	case kFitDriftBelowY: SetBelowDriftYOnly(); break;

	case kFitAllAboveX: SetAboveAllXOnly(); break;
	case kFitAllAboveY: SetAboveAllYOnly(); break;
	case kFitAllBelowX: SetBelowAllXOnly(); break;
	case kFitAllBelowY: SetBelowAllYOnly(); break;

	case kFitAllX: SetAllXOnly(); break;
	case kFitAllY: SetAllYOnly(); break;

	case kFitAll: SetUseAll(); break;

	default: break;
	}

}

void BristolPoCAFitter::SetUseAll() {
	fUseARPCX = true;
	fUseARPCY = true;
	fUseBRPCX = true;
	fUseBRPCY = true;
	fUseADriftX = true;
	fUseADriftY = true;
	fUseBDriftX = true;
	fUseBDriftY = true;
}


void BristolPoCAFitter::SetAboveAllXOnly() {
	fUseARPCX = true;
	fUseARPCY = false;
	fUseBRPCX = false;
	fUseBRPCY = false;
	fUseADriftX = true;
	fUseADriftY = false;
	fUseBDriftX = false;
	fUseBDriftY = false;
}

void BristolPoCAFitter::SetAboveAllYOnly() {
	fUseARPCX = false;
	fUseARPCY = true;
	fUseBRPCX = false;
	fUseBRPCY = false;
	fUseADriftX = false;
	fUseADriftY = true;
	fUseBDriftX = false;
	fUseBDriftY = false;
}

void BristolPoCAFitter::SetBelowAllXOnly() {
	fUseARPCX = false;
	fUseARPCY = false;
	fUseBRPCX = true;
	fUseBRPCY = false;
	fUseADriftX = false;
	fUseADriftY = false;
	fUseBDriftX = true;
	fUseBDriftY = false;
}

void BristolPoCAFitter::SetBelowAllYOnly() {
	fUseARPCX = false;
	fUseARPCY = false;
	fUseBRPCX = false;
	fUseBRPCY = true;
	fUseADriftX = false;
	fUseADriftY = false;
	fUseBDriftX = false;
	fUseBDriftY = true;
}



void BristolPoCAFitter::SetAllXOnly() {
	fUseARPCX = true;
	fUseARPCY = false;
	fUseBRPCX = true;
	fUseBRPCY = false;
	fUseADriftX = true;
	fUseADriftY = false;
	fUseBDriftX = true;
	fUseBDriftY = false;
}

void BristolPoCAFitter::SetAllYOnly() {
	fUseARPCX = false;
	fUseARPCY = true;
	fUseBRPCX = false;
	fUseBRPCY = true;
	fUseADriftX = false;
	fUseADriftY = true;
	fUseBDriftX = false;
	fUseBDriftY = true;
}

void BristolPoCAFitter::SetAboveDriftXOnly() {
	fUseARPCX = false;
	fUseARPCY = false;
	fUseBRPCX = false;
	fUseBRPCY = false;
	fUseADriftX = true;
	fUseADriftY = false;
	fUseBDriftX = false;
	fUseBDriftY = false;
}

void BristolPoCAFitter::SetAboveDriftYOnly() {
	fUseARPCX = false;
	fUseARPCY = false;
	fUseBRPCX = false;
	fUseBRPCY = false;
	fUseADriftX = false;
	fUseADriftY = true;
	fUseBDriftX = false;
	fUseBDriftY = false;
}

void BristolPoCAFitter::SetBelowDriftXOnly() {
	fUseARPCX = false;
	fUseARPCY = false;
	fUseBRPCX = false;
	fUseBRPCY = false;
	fUseADriftX = false;
	fUseADriftY = false;
	fUseBDriftX = true;
	fUseBDriftY = false;
}

void BristolPoCAFitter::SetBelowDriftYOnly() {
	fUseARPCX = false;
	fUseARPCY = false;
	fUseBRPCX = false;
	fUseBRPCY = false;
	fUseADriftX = false;
	fUseADriftY = false;
	fUseBDriftX = false;
	fUseBDriftY = true;
}




// ------------------------------------------------------------------------
// Single Track Fitter
//
// This is a bit awkward. Need to load in arbrirtrary sets of vectors
// and run the fit to them, whilst making the distinction between
// drift and rpc hits.
// ------------------------------------------------------------------------


std::vector<bool> BristolPoCAFitter::GetBestComboForDriftHits(int drifttype) {

	// Set data vectors depending on type
	FillSingleContainers(drifttype);

	// Get Combo Map
	std::vector<std::vector<bool> > xcombomapa;
	std::vector<bool> xcomboa;
	FillComboVect( xcombomapa, GetN() );

	// Loop over all iterations, find best chi2
	double bestchi2 = -1;
	int n = xcombomapa.size();

	for (int j = 0; j < n; j++) {

		// Update combination based on type
		SetVectorC( &xcombomapa[j] );

		// Get the results
		double chi2 = DoSingleTrackFitWithX();

		// If better than current, update
		if (bestchi2 < 0 or chi2 <= bestchi2) {
			xcomboa = xcombomapa[j];
			bestchi2 = chi2;
		}

	}

	return xcomboa;
}


double BristolPoCAFitter::DoSingleTrackFitWithX(double* fitx, double* fitpx, double* fitz) {

	// Create Minimizer Object
	ROOT::Math::Minimizer* min = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
	min->SetPrintLevel(-1);

	// Setup Functor for the fitter
	int npars = 2;
	BristolTrackFitterFCN* singlefcn = new BristolTrackFitterFCN(this);
	ROOT::Math::Functor func(*singlefcn, npars);
	min->SetFunction(func);

	// Get start pos and grad from hits
	double startx  = PredictStartX();
	// double startz  = values_rz.at(0);
	double startpx = PredictStartPX();

	// Run the fit
	min->SetVariable(0, "x",   startx,  0.1);
	min->SetVariable(1, "px1", 0.0, 10);
	// min->SetVariable(2, "z",   startz,  0.1);
	// min->FixVariable(2);
	min->Minimize();

	// Get the results
	const double* xx = min->X();
	double chi2 = DoSingleEvalWithX(xx);
	if (fitx)   *fitx = xx[0];
	if (fitpx) *fitpx = xx[1];
	// if (fitz)   *fitz = xx[2];

	delete min;

	return chi2;
}

void BristolPoCAFitter::FillComboVect(std::vector<std::vector<bool> >& combomap, int n) {

	for (int i = 0; i < (1 << n); i++) {
		std::vector<bool> combo;
		for (int j = 0; j < n; j++) {
			combo.push_back(((i & (1 << j)) ? 1 : 0));
		}
		combomap.push_back(combo);
	}

}


double BristolPoCAFitter::DoSingleEvalWithX(const double *x) const {

	double chi2 = 0.0;

	double pointx = x[0];
	double momx   = x[1];
	double pointz = 0.0; //values_z.at(0);
	double momz   = 1.0;

	for (uint i = 0; i < values_x.size(); i++) {
		if (values_c->at(i)) {
			chi2 += pow( ( values_g.at(i) - (pointx + momx * (pointz - values_z.at(i)) / momz) ) / values_e.at(i), 2 );
		} else {
			chi2 += pow( ( values_x.at(i) - (pointx + momx * (pointz - values_z.at(i)) / momz) ) / values_e.at(i), 2 );
		}
	}

	for (uint i = 0; i < values_rx.size(); i++) {
		chi2 += pow( ( values_rx.at(i) - (pointx + momx * (pointz - values_rz.at(i)) / momz) ) / values_re.at(i), 2 );
	}

	// std::cout << " Single Chi2 = " << chi2 << std::endl;

	return chi2;
}


void BristolPoCAFitter::PerformDoubleTrackPoCAFit(double* pocafitparams) {

	// Get fit values
	double temp_above_x = 0.0;
	double temp_above_px = 0.0;
	double temp_above_xz = 0.0;
	double temp_above_y = 0.0;
	double temp_above_py = 0.0;
	double temp_above_yz = 0.0;

	double temp_below_x = 0.0;
	double temp_below_px = 0.0;
	double temp_below_xz = 0.0;
	double temp_below_y = 0.0;
	double temp_below_py = 0.0;
	double temp_below_yz = 0.0;

	// Perform an Upper X fit
	FillSingleContainers(kFitAllAboveX);
	SetVectorC( above_drift_xc );
	DoSingleTrackFitWithX(&temp_above_x, &temp_above_px, &temp_above_xz);

	// Perform an Upper Y fit
	FillSingleContainers(kFitAllAboveY);
	SetVectorC( above_drift_yc );
	DoSingleTrackFitWithX(&temp_above_y, &temp_above_py, &temp_above_yz);

	// Perform an Lower X fit
	FillSingleContainers(kFitAllBelowX);
	SetVectorC( below_drift_xc );
	DoSingleTrackFitWithX(&temp_below_x, &temp_below_px, &temp_below_xz);

	// Perform an Lower Y fit
	FillSingleContainers(kFitAllBelowY);
	SetVectorC( below_drift_yc );
	DoSingleTrackFitWithX(&temp_below_y, &temp_below_py, &temp_below_yz);

	// Get vectors from the fits
	TVector3 posA= TVector3(temp_above_x, temp_above_y, 0.0);
	TVector3 momA= TVector3(temp_above_px, temp_above_py, 1.0);

	TVector3 posB= TVector3(temp_below_x, temp_below_y, 0.0);
	TVector3 momB= TVector3(temp_below_px, temp_below_py, 1.0);

    // Get the PoCA
	TVector3 w0 = posA - posB;
    double a = momA.Dot(momA);
    double b = momA.Dot(momB);
    double c = momB.Dot(momB);
    double d = momA.Dot(w0);
    double e = momB.Dot(w0);

    double sc = ((b*e) - (c*d))/((a*c)-(b*b));
    double tc = ((a*e) - (b*d))/((a*c)-(b*b));

    TVector3 poca = 0.5 * (posA + sc*momA + posB + tc*momB);

    // Get the distance between points of closest approach
    double distance = (posA + sc*momB - posB - tc*momB).Mag();

    // Get the scatter angles
    TVector3 grad1X(temp_above_px, 0., 1.);
    TVector3 grad2X(temp_below_px, 0., 1.);

    TVector3 grad1Y(0., temp_above_py, 1.);
    TVector3 grad2Y(0., temp_below_py, 1.);

    TVector3 grad1(temp_above_px, temp_above_py, 1.);
    TVector3 grad2(temp_below_px, temp_below_py, 1.);

	double scatter_x  = grad1X.Angle(grad2X);
	double scatter_y  = grad1Y.Angle(grad2Y); 
    double scatter_3d = grad2.Angle(grad1);

	// Fill variables and return
	pocafitparams[0] = temp_above_x;
	pocafitparams[1] = temp_above_px;
	pocafitparams[2] = temp_above_y;
	pocafitparams[3] = temp_above_py;
	pocafitparams[4] = temp_below_x;
	pocafitparams[5] = temp_below_px;
	pocafitparams[6] = temp_below_y;
	pocafitparams[7] = temp_below_py;

	pocafitparams[8]  = atan( abs((temp_above_px - temp_below_px)/(1 + temp_above_px*temp_below_px)) );
	pocafitparams[9]  = atan( abs((temp_above_py - temp_below_py)/(1 + temp_above_py*temp_below_py)) );
	pocafitparams[10] = scatter_3d;

	pocafitparams[11] = poca[0];
	pocafitparams[12] = poca[1];
	pocafitparams[13] = -poca[2];

	return;
}

double BristolPoCAFitter::GetLowestXXAbove(){
	FillSingleContainers(kFitAllAboveX);
	SetVectorC( above_drift_xc );
	return GetLowestX();
}

double BristolPoCAFitter::GetLowestXZAbove(){
	FillSingleContainers(kFitAllAboveX);
	SetVectorC( above_drift_xc );
	return GetLowestZ();
}

double BristolPoCAFitter::GetLowestYYAbove(){
	FillSingleContainers(kFitAllAboveY);
	SetVectorC( above_drift_yc );
	return GetLowestX();
}

double BristolPoCAFitter::GetLowestYZAbove(){
	FillSingleContainers(kFitAllAboveY);
	SetVectorC( above_drift_yc );
	return GetLowestZ();
}

double BristolPoCAFitter::GetHighestXXBelow(){
	FillSingleContainers(kFitAllBelowX);
	SetVectorC( below_drift_xc );
	return GetHighestX();
}

double BristolPoCAFitter::GetHighestXZBelow(){
	FillSingleContainers(kFitAllBelowX);
	SetVectorC( below_drift_xc );
	return GetHighestZ();
}

double BristolPoCAFitter::GetHighestYYBelow(){
	FillSingleContainers(kFitAllBelowY);
	SetVectorC( below_drift_yc );
	return GetHighestX();
}

double BristolPoCAFitter::GetHighestYZBelow(){
	FillSingleContainers(kFitAllBelowY);
	SetVectorC( below_drift_yc );
	return GetHighestZ();
}

}










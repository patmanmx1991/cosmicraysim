#ifndef __BRISTOLPOCA_FITTER_HH__
#define __BRISTOLPOCA_FITTER_HH__


#include "GeoObject.hh"
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
#include "GeoUtils.hh"
#include "sd/DetectorManager.hh"
#include "simple/GeoBox.hh"
#include "simple/GeoTubs.hh"
#include "sd/LongDriftSD.hh"
#include "sd/AWEDriftSD.hh"
#include "sd/BristolRPCSD.hh"

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
#include "db/ROOTHeaders.hh"
#include "G4Polyline.hh"
#include "Math/Functor.h"

namespace COSMIC {


class BristolPoCAFitter {
public:

	BristolPoCAFitter();
	~BristolPoCAFitter();

	int FitTracks();

	void ReadInputTTree(TTree* t, std::string prefixa = "system", std::string prefixb="system_above");
	void AddToOutputTTree(TTree* t, std::string prefix = "system");
	void FillOutputTree();

	void DeleteContainers();

	double DoEval(const double* x) const;

	virtual double operator()( const double* x ) const { return DoEval(x); };

	int GetStatus(){return fStatus;};

protected:

	std::vector<double>* above_rpc_xx;
	std::vector<double>* above_rpc_xt;
	std::vector<double>* above_rpc_xz;
	std::vector<double>* above_rpc_xe;
	std::vector<double>* above_rpc_yy;
	std::vector<double>* above_rpc_yt;
	std::vector<double>* above_rpc_yz;
	std::vector<double>* above_rpc_ye;

	std::vector<double>* above_drift_xx;
	std::vector<double>* above_drift_xg;
	std::vector<double>* above_drift_xt;
	std::vector<double>* above_drift_xz;
	std::vector<double>* above_drift_xe;
	std::vector<bool>*   above_drift_xc;

	std::vector<double>* above_drift_yy;
	std::vector<double>* above_drift_yg;
	std::vector<double>* above_drift_yt;
	std::vector<double>* above_drift_yz;
	std::vector<double>* above_drift_ye;
	std::vector<bool>*   above_drift_yc;

	std::vector<double>* below_rpc_xx;
	std::vector<double>* below_rpc_xt;
	std::vector<double>* below_rpc_xz;
	std::vector<double>* below_rpc_xe;
	std::vector<double>* below_rpc_yy;
	std::vector<double>* below_rpc_yt;
	std::vector<double>* below_rpc_yz;
	std::vector<double>* below_rpc_ye;

	std::vector<double>* below_drift_xx;
	std::vector<double>* below_drift_xg;
	std::vector<double>* below_drift_xt;
	std::vector<double>* below_drift_xz;
	std::vector<double>* below_drift_xe;
	std::vector<bool>*   below_drift_xc;

	std::vector<double>* below_drift_yy;
	std::vector<double>* below_drift_yg;
	std::vector<double>* below_drift_yt;
	std::vector<double>* below_drift_yz;
	std::vector<double>* below_drift_ye;
	std::vector<bool>*   below_drift_yc;

	bool fUseRPC;
	bool fUseDrift;

	double fBestFitPars[7];
	double fCovarMatrix[49];
	double fMinuitParams[6];
	double fErrorDef;

	uint fMinARPCX;
	uint fMinARPCY;
	uint fMinBRPCX;
	uint fMinBRPCY;
	uint fMinADriftX;
	uint fMinADriftY;
	uint fMinBDriftX;
	uint fMinBDriftY;

	int fStatus;
};

class BristolPoCAFitterFCN {
public:
	inline BristolPoCAFitterFCN(BristolPoCAFitter* f) : fFitter(f) {};
		// fFitter = f;
	// }
	inline double operator() (const double *x) const {
		return fFitter->DoEval(x);
	}
	BristolPoCAFitter* fFitter;
};

}
#endif

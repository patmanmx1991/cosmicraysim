#ifndef __BRISTOLSINGLETRACK_FITTER_HH__
#define __BRISTOLSINGLETRACK_FITTER_HH__


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
#include "G4Polyline.hh"

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
#include "Math/Functor.h"

#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "sd/DetectorManager.hh"
#include "geo/simple/GeoBox.hh"
#include "geo/simple/GeoTubs.hh"
#include "sd/LongDriftSD.hh"
//#include "sd/AWEDriftSD.hh"
//#include "sd/BristolRPCSD.hh"
#include "db/ROOTHeaders.hh"

namespace COSMIC {


class BristolSingleTrackFitter {
public:

	BristolSingleTrackFitter();
	~BristolSingleTrackFitter();

	int FitTracks();

	void ReadInputTTree(TTree* t, std::string prefix = "system");
	void AddToOutputTTree(TTree* t, std::string prefix = "system");
	void FillOutputTree();

	void DeleteContainers();
	void SetRPCState(bool use);
	void SetDriftState(bool use);

	double DoEval(const double* x) const;

	virtual double operator()( const double* x ) const { return DoEval(x); };
	virtual double operator()( const std::vector<double>& par ) const { return DoEval(&par[0]); };
	virtual double Up() const { return fErrorDef; }


	void SetRPCXX(std::vector<double>* rpcx) { system_rpc_xx = rpcx; };
	void SetRPCXT(std::vector<double>* rpcx) { system_rpc_xt = rpcx; };
	void SetRPCXE(std::vector<double>* rpcx) { system_rpc_xe = rpcx; };
	void SetRPCXZ(std::vector<double>* rpcx) { system_rpc_xz = rpcx; };

	void SetRPCYY(std::vector<double>* rpcy) { system_rpc_yy = rpcy; };
	void SetRPCYT(std::vector<double>* rpcy) { system_rpc_yy = rpcy; };
	void SetRPCYE(std::vector<double>* rpcy) { system_rpc_yy = rpcy; };
	void SetRPCYZ(std::vector<double>* rpcy) { system_rpc_yy = rpcy; };

	void SetDriftXX(std::vector<double>* drfx) { system_drift_xx = drfx; };
	void SetDriftXG(std::vector<double>* drfx) { system_drift_xg = drfx; };
	void SetDriftXT(std::vector<double>* drfx) { system_drift_xt = drfx; };
	void SetDriftXZ(std::vector<double>* drfx) { system_drift_xz = drfx; };
	void SetDriftXE(std::vector<double>* drfx) { system_drift_xe = drfx; };
	void SetDriftXC(std::vector<bool>*   drfx) { system_drift_xc = drfx; };

	void SetDriftYY(std::vector<double>* drfy) { system_drift_yy = drfy; };
	void SetDriftYG(std::vector<double>* drfy) { system_drift_yg = drfy; };
	void SetDriftYT(std::vector<double>* drfy) { system_drift_yt = drfy; };
	void SetDriftYZ(std::vector<double>* drfy) { system_drift_yz = drfy; };
	void SetDriftYE(std::vector<double>* drfy) { system_drift_ye = drfy; };
	void SetDriftYC(std::vector<bool>*   drfy) { system_drift_yc = drfy; };

	std::vector<double>* GetRPCXX() { return system_rpc_xx; };
	std::vector<double>* GetRPCXT() { return system_rpc_xt; };
	std::vector<double>* GetRPCXE() { return system_rpc_xe; };
	std::vector<double>* GetRPCXZ() { return system_rpc_xz; };

	std::vector<double>* GetRPCYY() { return system_rpc_yy; };
	std::vector<double>* GetRPCYT() { return system_rpc_yt; };
	std::vector<double>* GetRPCYE() { return system_rpc_ye; };
	std::vector<double>* GetRPCYZ() { return system_rpc_yz; };

	std::vector<double>* GetDriftXX() { return system_drift_xx; };
	std::vector<double>* GetDriftXG() { return system_drift_xg; };
	std::vector<double>* GetDriftXT() { return system_drift_xt; };
	std::vector<double>* GetDriftXZ() { return system_drift_xz; };
	std::vector<double>* GetDriftXE() { return system_drift_xe; };

	std::vector<double>* GetDriftYY() { return system_drift_yy; };
	std::vector<double>* GetDriftYG() { return system_drift_yg; };
	std::vector<double>* GetDriftYT() { return system_drift_yt; };
	std::vector<double>* GetDriftYZ() { return system_drift_yz; };
	std::vector<double>* GetDriftYE() { return system_drift_ye; };

	int GetStatus(){return fStatus;};

protected:

	std::vector<double>* system_rpc_xx;
	std::vector<double>* system_rpc_xt;
	std::vector<double>* system_rpc_xz;
	std::vector<double>* system_rpc_xe;
	std::vector<double>* system_rpc_yy;
	std::vector<double>* system_rpc_yt;
	std::vector<double>* system_rpc_yz;
	std::vector<double>* system_rpc_ye;

	std::vector<double>* system_drift_xx;
	std::vector<double>* system_drift_xg;
	std::vector<double>* system_drift_xt;
	std::vector<double>* system_drift_xz;
	std::vector<double>* system_drift_xe;
	std::vector<bool>*   system_drift_xc;

	std::vector<double>* system_drift_yy;
	std::vector<double>* system_drift_yg;
	std::vector<double>* system_drift_yt;
	std::vector<double>* system_drift_yz;
	std::vector<double>* system_drift_ye;
	std::vector<bool>*   system_drift_yc;

	bool fRPCState;
	bool fUseRPC;
	bool fDriftState;
	bool fUseDrift;
	double fBestFitPars[5];
	double fCovarMatrix[25];
	double fMinuitParams[6];
	double fErrorDef;

	uint fMinRPCX;
	uint fMinRPCY;
	uint fMinDriftX;
	uint fMinDriftY;

	int fStatus;
};

class BristolSingleTrackFitterFCN {
public:
	inline BristolSingleTrackFitterFCN(BristolSingleTrackFitter* f) : fFitter(f) {};
		// fFitter = f;
	// }
	inline double operator() (const double *x) const {
		return fFitter->DoEval(x);
	}
	BristolSingleTrackFitter* fFitter;
	// inline double DoEval(const double *x) const {
		// return fFitter->DoEval(x);
	// }
	// inline double Up() const {
	// 	return 1.0;
	// }
};

}
#endif

#ifndef __BRISTOLPOCA_FITTER_HH__
#define __BRISTOLPOCA_FITTER_HH__


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
//#include "sd/AWEDriftSD.hh"
//#include "sd/BristolRPCSD.hh"

#include <cassert>
#include "Minuit2/FCNBase.h"



namespace COSMIC {




class BristolPoCAFitter : public ROOT::Minuit2::FCNBase {
public:

	BristolPoCAFitter();
	~BristolPoCAFitter();

	int FitTracks();

	void ReadInputTTree(TTree* t, std::string prefixa = "system", std::string prefixb = "system_above");
	void AddToOutputTTree(TTree* t, std::string prefix = "system");
	void FillOutputTree();

	void DeleteContainers();

	double DoEval(const double* x) const;

	virtual double operator()( const double* x ) const { return DoEval(x); };
	virtual double operator()( const std::vector<double>& par ) const{
		return DoEval( &par[0] );
	};
	virtual double Up() const { return 1.0; };


	int GetStatus() {return fStatus;};

	int GetNAboveRPCX()  { return above_rpc_xx->size();   };
	int GetNAboveRPCY()  { return above_rpc_yy->size();   };
	int GetNBelowRPCX()  { return below_rpc_xx->size();   };
	int GetNBelowRPCY()  { return below_rpc_yy->size();   };

	int GetNAboveDriftX() { return above_drift_xx->size(); };
	int GetNAboveDriftY() { return above_drift_yy->size(); };
	int GetNBelowDriftX() { return below_drift_xx->size(); };
	int GetNBelowDriftY() { return below_drift_yy->size(); };

	void SetAboveComboX(std::vector<bool>* co) { above_drift_xc = co; };
	void SetAboveComboY(std::vector<bool>* co) { above_drift_yc = co; };
	void SetBelowComboX(std::vector<bool>* co) { below_drift_xc = co; };
	void SetBelowComboY(std::vector<bool>* co) { below_drift_yc = co; };

	void SetAboveDriftXOnly();
	void SetAboveDriftYOnly();
	void SetBelowDriftXOnly();
	void SetBelowDriftYOnly();

	std::vector<double>* GetAXX() { return above_drift_xx; };
	std::vector<double>* GetAXG() { return above_drift_xg; };
	std::vector<double>* GetAXZ() { return above_drift_xz; };

	std::vector<double>* GetAYY() { return above_drift_yy; };
	std::vector<double>* GetAYG() { return above_drift_yg; };
	std::vector<double>* GetAYZ() { return above_drift_yz; };

	std::vector<double>* GetBXX() { return below_drift_xx; };
	std::vector<double>* GetBXG() { return below_drift_xg; };
	std::vector<double>* GetBXZ() { return below_drift_xz; };

	std::vector<double>* GetBYY() { return below_drift_yy; };
	std::vector<double>* GetBYG() { return below_drift_yg; };
	std::vector<double>* GetBYZ() { return below_drift_yz; };

	double GetChi2AboveRPCX( double pointx, double momx1, double pointz ) const ;
	double GetChi2AboveRPCY( double pointy, double momy1, double pointz ) const ;

	double GetChi2AboveDriftX( double pointx, double momx1, double pointz ) const ;
	double GetChi2AboveDriftY( double pointy, double momy1, double pointz ) const ;

	double GetChi2BelowRPCX( double pointx, double momx1, double pointz ) const ;
	double GetChi2BelowRPCY( double pointy, double momy1, double pointz ) const ;

	double GetChi2BelowDriftX( double pointx, double momx1, double pointz ) const ;
	double GetChi2BelowDriftY( double pointy, double momy1, double pointz ) const ;

	void PerformDoubleTrackPoCAFit(double* pocafitparams);
	double GetLowestX();
	double GetLowestZ();
	double GetHighestX();
	double GetHighestZ();



	double GetLowestXXAbove();
	double GetLowestXZAbove();
	double GetLowestYYAbove();
	double GetLowestYZAbove();

	double GetHighestXXBelow();
	double GetHighestXZBelow();
	double GetHighestYYBelow();
	double GetHighestYZBelow();

	std::vector<bool> GetBestComboForDriftHits(int combo);

	enum {
		kFitDriftAboveX,
		kFitDriftAboveY,
		kFitDriftBelowX,
		kFitDriftBelowY,
		kFitAllAboveX,
		kFitAllAboveY,
		kFitAllBelowX,
		kFitAllBelowY,
		kFitAllX,
		kFitAllY,
		kFitAll
	};

	double DoSingleEvalWithX(const double *x) const;

	double DoSingleTrackFitWithX(double* fitx = 0, double* fitpx = 0, double* fitz = 0);

	void SetVectorC(std::vector<bool>*   c) { values_c = c; };

	int GetN() { return values_x.size(); };

	double PredictStartX();
	double PredictStartZ();

	double PredictStartPX();
	void FillComboVect(std::vector<std::vector<bool> >& combomap, int n);


	void SetHitUsage(int drifttype);
	void FillSingleContainers(int drifttype);
	void ClearSingleContainers();

	double PredictStartAbovePX();
	double PredictStartAbovePY();
	double PredictStartBelowPX();
	double PredictStartBelowPY();

	double PredictStartPoCAX();
	double PredictStartPoCAY();
	double PredictStartPoCAZ();

	void SetAboveAllXOnly();
	void SetAboveAllYOnly();
	void SetBelowAllXOnly();
	void SetBelowAllYOnly();

	void SetAllXOnly();
	void SetAllYOnly();

	void SetUseAll();

	std::vector<double> values_x;
	std::vector<double> values_e;
	std::vector<double> values_g;
	std::vector<double> values_z;
	std::vector<bool>*  values_c;

	std::vector<double> values_rx;
	std::vector<double> values_re;
	std::vector<double> values_rz;


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

	bool fUseARPCX;
	bool fUseARPCY;
	bool fUseBRPCX;
	bool fUseBRPCY;
	bool fUseADriftX;
	bool fUseADriftY;
	bool fUseBDriftX;
	bool fUseBDriftY;

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
	inline double operator() (const double *x) const {
		return fFitter->DoEval(x);
	}
	BristolPoCAFitter* fFitter;
};

class BristolTrackFitterFCN {
public:
	inline BristolTrackFitterFCN(BristolPoCAFitter* f) : fFitter(f) {};
	inline double operator() (const double *x) const {
		return fFitter->DoSingleEvalWithX(x);
	}
	BristolPoCAFitter* fFitter;
};

}
#endif













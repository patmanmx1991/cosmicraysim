#ifndef __AWEMuonTomographyDetector_HH__
#define __AWEMuonTomographyDetector_HH__

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
#include "GeoObject.hh"
#include "GeoUtils.hh"
#include "analysis/VProcessor.hh"
#include "analysis/Analysis.hh"


#include "db/ROOTHeaders.hh"


#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>



#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "sd/LongDriftSD.hh"
#include "sd/SimpleScintillatorSD.hh"
#include "TH1.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TVirtualFitter.h"
#include "TStyle.h"
#include "Minuit2/FCNBase.h"
#include "TFitterMinuit.h"
#include "TSystem.h"
#include "TStopwatch.h"

#include <vector>
#include <iostream>


class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

// double inheritance, super dodgy!
class AWEMuonTomographyDetector : public GeoObject {
public:
  inline AWEMuonTomographyDetector() {};
  inline AWEMuonTomographyDetector(DBTable table) {Construct(table);};
  void Construct(DBTable table);

  std::vector<GeoObject*> GetDriftObjects() { return fDriftObjects; };
  GeoObject* GetScintillatorObject() { return fScintObject; };

protected:
  std::vector<GeoObject*> fSubObjects;
  std::vector<GeoObject*> fDriftObjects;
  GeoObject* fScintObject;

};


//! Wrapper for JointFCN to make ROOT minimization behave sensibly.
class TrackFitter  {
public:

  // Empty Construction
  TrackFitter() {}
  ~TrackFitter() {};
  // double operator() (const std::vector<double> & x) const {
  // return DoEval(&(x[0]));
  // }
  // Func Operator for vectors
  // inline double operator() (const std::vector<double> & x) const
  // {
  // double* x_array = new double[x.size()];
  // return this->DoEval(x_array);
  // };

  // Actual Fitter
  double DoEvalS(const double *x) const;

  // Func Operator for arrays
  inline double operator() (const double *x) const
  {
    return this->DoEvalS(x);
  };

private:


};


/// True Muon Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class AWEMuonTomographyProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  AWEMuonTomographyProcessor(AWEMuonTomographyDetector* trkr);
  /// Destructor
  ~AWEMuonTomographyProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);


  void GetMXC(G4double& m, G4double& c, std::vector<G4double>& x, std::vector<G4double>& y, std::vector<G4double>& yerr);



protected:

  AWEMuonTomographyDetector* fAWEDetector; ///< Pointer to associated detector medium
  std::vector<LongDriftProcessor*> fDriftChamberProcs;
  SimpleScintillatorProcessor* fScintProc;

  int fMuonTimeIndex; ///< Time Ntuple Index
  int fMuonMomXIndex; ///< MomX Ntuple Index
  int fMuonMomYIndex; ///< MomY Ntuple Index
  int fMuonMomZIndex; ///< MomZ Ntuple Index
  int fMuonPosXIndex; ///< PosX Ntuple Index
  int fMuonPosYIndex; ///< PosY Ntuple Index
  int fMuonPosZIndex; ///< PosZ Ntuple Index
  int fMuonPDGIndex;  ///< MPDG Ntuple Index

  // Create Fit Machinery
  TrackFitter* fFitterFCN;
  ROOT::Math::Functor* fCallFunctor;
  ROOT::Math::Minimizer* fMinimizer;
  G4ThreeVector fMuonMom;
  G4ThreeVector fMuonMomErr;
  G4ThreeVector fMuonPos;
  G4ThreeVector fMuonPosErr;

};






} // namespace COSMIC
#endif

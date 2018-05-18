#ifndef __HybridMuonTomographyDetector_HH__
#define __HybridMuonTomographyDetector_HH__

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
#include "sd/AWEDriftSD.hh"
#include "sd/BristolRPCSD.hh"

#include "sd/SimpleScintillatorSD.hh"
#include "TH1.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TVirtualFitter.h"
#include "TSystem.h"
#include "TStopwatch.h"

#include <vector>
#include <iostream>

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

// double inheritance, super dodgy!
class HybridMuonTomographyDetector : public GeoObject {
public:
  inline HybridMuonTomographyDetector() {};
  inline HybridMuonTomographyDetector(DBTable table) {Construct(table);};

  void Construct(DBTable table);

  std::vector<GeoObject*> GetDriftObjects() { return fDriftObjects; };

  GeoObject* GetScintillatorTop() { return fScintObjectTop; };
  GeoObject* GetScintillatorBottom() { return fScintObjectBottom; };

  GeoObject* GetRPCTopX() { return fRPCTopX; };
  GeoObject* GetRPCTopY() { return fRPCTopY; };

  GeoObject* GetRPCBottomX() { return fRPCBottomX; };
  GeoObject* GetRPCBottomY() { return fRPCBottomY; };

protected:

  std::vector<GeoObject*> fSubObjects;
  std::vector<GeoObject*> fDriftObjects;

  GeoObject* fScintObjectTop;
  GeoObject* fScintObjectBottom;

  GeoObject* fRPCTopX;
  GeoObject* fRPCTopY;

  GeoObject* fRPCBottomX;
  GeoObject* fRPCBottomY;
};

/// Hybrid Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class HybridMuonTomographyProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  HybridMuonTomographyProcessor(HybridMuonTomographyDetector* trkr);
  /// Destructor
  ~HybridMuonTomographyProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);
  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Fit a set of XY points
  void GetMXC(G4double& m, G4double& me, G4double& c, G4double& ce, std::vector<G4double>& x, std::vector<G4double>& y, std::vector<G4double>& yerr);
void ResetVariables();


protected:

  bool fSave; ///< Whether to auto save

  HybridMuonTomographyDetector* fHybridDetector; ///< Pointer to associated detector medium
  std::vector<AWEDriftProcessor*> fDriftChamberProcs; ///< List of all drift processors

  BristolRPCProcessor* fRPCProcTopX; ///< Processor for the Bristol RPC Hits above
  BristolRPCProcessor* fRPCProcBottomX; ///< Processor for the Bristol RPC hits below
  BristolRPCProcessor* fRPCProcTopY; ///< Processor for the Bristol RPC Hits above
  BristolRPCProcessor* fRPCProcBottomY; ///< Processor for the Bristol RPC hits below

  SimpleScintillatorProcessor* fScintProcTop; ///< Scintillator trigger processor
  SimpleScintillatorProcessor* fScintProcBottom; ///< Scintillator trigger processor

  int fScintTopEIndex; ///< Time Ntuple Index
  int fScintTopTIndex; ///< Time Ntuple Index
  int fScintBottomEIndex; ///< Time Ntuple Index
  int fScintBottomTIndex; ///< Time Ntuple Index

  G4double fScintTopE;
  G4double fScintTopT;
  G4double fScintBottomT;
  G4double fScintBottomE;

  std::vector<double> fRPCHits_XX;
  std::vector<double> fRPCHits_XT;
  std::vector<double> fRPCHits_XZ;
  std::vector<double> fRPCHits_XE;

  std::vector<double> fRPCHits_YY;
  std::vector<double> fRPCHits_YT;
  std::vector<double> fRPCHits_YZ;
  std::vector<double> fRPCHits_YE;

  std::vector<double> fDriftHits_XX;
  std::vector<double> fDriftHits_XG;
  std::vector<double> fDriftHits_XT;
  std::vector<double> fDriftHits_XZ;
  std::vector<double> fDriftHits_XE;

  std::vector<double> fDriftHits_YY;
  std::vector<double> fDriftHits_YG;
  std::vector<double> fDriftHits_YT;
  std::vector<double> fDriftHits_YZ;
  std::vector<double> fDriftHits_YE;

};






} // namespace COSMIC
#endif

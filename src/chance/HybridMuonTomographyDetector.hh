#ifndef __HybridMuonTomographyDetector_HH__
#define __HybridMuonTomographyDetector_HH__

#include <vector>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>

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

#include "db/ROOTHeaders.hh"
#include "sd/LongDriftSD.hh"
#include "chance/AWEDriftSD.hh"
#include "chance/BristolRPCSD.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoUtils.hh"
#include "analysis/VProcessor.hh"
#include "analysis/Analysis.hh"
#include "sd/SimpleScintillatorSD.hh"




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
  std::vector<GeoObject*> GetScintObjects() { return fScintObjects; };
  std::vector<GeoObject*> GetRPCObjects() { return fRPCObjects; };

protected:

  std::vector<GeoObject*> fSubObjects;
  std::vector<GeoObject*> fDriftObjects;
  std::vector<GeoObject*> fScintObjects;
  std::vector<GeoObject*> fRPCObjects;

};

/// Hybrid Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class HybridMuonTomographyProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  HybridMuonTomographyProcessor(HybridMuonTomographyDetector* trkr, std::map<std::string, std::string>& hitinfo);
  /// Destructor
  ~HybridMuonTomographyProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);
  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Fit a set of XY points
  void GetMXC(G4double& m, G4double& me, G4double& c, G4double& ce, std::vector<G4double>& x, std::vector<G4double>& y, std::vector<G4double>& yerr);
void ResetVariables();

  int ConvertHitInfo(std::string s);

  enum {
    kHitInfoX = 0,
    kHitInfoY,
  };

protected:

  bool fSave; ///< Whether to auto save

  HybridMuonTomographyDetector* fHybridDetector; ///< Pointer to associated detector medium

  std::vector<AWEDriftProcessor*> fDriftChamberProcs; ///< List of all drift processors
  std::vector<int> fDriftHitInfo;

  std::vector<SimpleScintillatorProcessor*> fScintProcs; ///< List of all drift processors

  std::vector<BristolRPCProcessor*> fRPCProcs; ///< List of all drift processors
  std::vector<int> fRPCHitInfo;


  std::vector<double> fScintE;
  std::vector<double> fScintT;

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

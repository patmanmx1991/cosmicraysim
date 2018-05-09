#ifndef __AWEMuonTomographyDetectorSecond_HH__
#define __AWEMuonTomographyDetectorSecond_HH__

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

#include "sd/SimpleScintillatorSD.hh"
#include "TH1.h"
#include "TF1.h"
#include "TRandom3.h"
#include "TVirtualFitter.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TStopwatch.h"

#include <vector>
#include <iostream>


class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

// double inheritance, super dodgy!
class AWEMuonTomographyDetectorSecond : public GeoObject {
public:
  inline AWEMuonTomographyDetectorSecond() {};
  inline AWEMuonTomographyDetectorSecond(DBTable table) {Construct(table);};
  void Construct(DBTable table);

  std::vector<GeoObject*> GetDriftObjects() { return fDriftObjects; };
  GeoObject* GetScintillatorTop() { return fScintObjectTop; };
  GeoObject* GetScintillatorBottom() { return fScintObjectBottom; };

protected:
  std::vector<GeoObject*> fSubObjects;
  std::vector<GeoObject*> fDriftObjects;
  GeoObject* fScintObjectTop;
  GeoObject* fScintObjectBottom;

};

/// True Muon Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class AWEMuonTomographyProcessorSecond : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  AWEMuonTomographyProcessorSecond(AWEMuonTomographyDetectorSecond* trkr);
  /// Destructor
  ~AWEMuonTomographyProcessorSecond() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);


  void GetMXC(G4double& m, G4double& me, G4double& c, G4double& ce, std::vector<G4double>& x, std::vector<G4double>& y, std::vector<G4double>& yerr);

protected:

  bool fSave; ///< Whether to auto save

  AWEMuonTomographyDetectorSecond* fAWEDetector; ///< Pointer to associated detector medium
  std::vector<AWEDriftProcessor*> fDriftChamberProcs; ///< List of all drift processors
  SimpleScintillatorProcessor* fScintProcTop; ///< Scintillator trigger processor
  SimpleScintillatorProcessor* fScintProcBottom; ///< Scintillator trigger processor

  int fMuonTimeIndex; ///< Time Ntuple Index
  int fMuonMomXIndex; ///< MomX Ntuple Index
  int fMuonMomYIndex; ///< MomY Ntuple Index
  int fMuonMomZIndex; ///< MomZ Ntuple Index
  int fMuonMomErrXIndex; ///< MomX Ntuple Index
  int fMuonMomErrYIndex; ///< MomY Ntuple Index
  int fMuonMomErrZIndex; ///< MomZ Ntuple Index
  int fMuonPosXIndex; ///< PosX Ntuple Index
  int fMuonPosYIndex; ///< PosY Ntuple Index
  int fMuonPosZIndex; ///< PosZ Ntuple Index
  int fMuonPosErrXIndex; ///< PosX Ntuple Index
  int fMuonPosErrYIndex; ///< PosY Ntuple Index
  int fMuonPosErrZIndex; ///< PosZ Ntuple Index
  int fMuonPDGIndex;  ///< MPDG Ntuple Index

  // Outputs
  G4ThreeVector fMuonMom;
  G4ThreeVector fMuonMomErr;
  G4ThreeVector fMuonPos;
  G4ThreeVector fMuonPosErr;

};






} // namespace COSMIC
#endif

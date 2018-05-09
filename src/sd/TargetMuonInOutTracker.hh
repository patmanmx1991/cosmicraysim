#ifndef __TargetMuonInOutTracker_HH__
#define __TargetMuonInOutTracker_HH__

#include "G4UserRunAction.hh"
#include "globals.hh"

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
#include "G4GlobalMagFieldMessenger.hh"
#include "G4Run.hh"
#include "globals.hh"
#include "g4root.hh"
#include "analysis/VProcessor.hh"
#include "analysis/VDetector.hh"

namespace COSMIC {

// Forward Declarations
class DBTable;

/// True Muon Tracker Detector Object :
/// Saves the highest momentrum muon track
/// that entered a detector logical voluem
class TargetMuonInOutTracker : public VDetector {
public:

  /// Main constructor from a database table
  TargetMuonInOutTracker(DBTable table);
  /// Simple C++ constructor
  TargetMuonInOutTracker(std::string name, std::string id, bool autoprocess=true, bool autosave=true);
  /// Destructor
  ~TargetMuonInOutTracker(){};

  /// Main processing. Looks for highest momentum track.
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  // Getter Functions
  inline G4double       GetMuonInTime(){ return fInTime; };
  inline G4ThreeVector  GetMuonInMom(){ return fInMom;  };
  inline G4ThreeVector  GetMuonInPos(){ return fInPos;  };
  inline G4double       GetMuonOutTime(){ return fOutTime; };
  inline G4ThreeVector  GetMuonOutMom(){ return fOutMom;  };
  inline G4ThreeVector  GetMuonOutPos(){ return fOutPos;  };

protected:
  
  G4double      fInTime; ///< HM Muon Step Time
  G4ThreeVector fInMom;  ///< HM Muon Mom Vector
  G4ThreeVector fInPos;  ///< HM Muon Pos Vector

  G4double      fOutTime; ///< HM Muon Step Time
  G4ThreeVector fOutMom;  ///< HM Muon Mom Vector
  G4ThreeVector fOutPos;  ///< HM Muon Pos Vector
  bool fFoundInStep;
  bool fFoundOutStep;

};

/// True Muon Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class TargetMuonInOutProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  TargetMuonInOutProcessor(TargetMuonInOutTracker* trkr, bool autosave=true);
  /// Destructor
  ~TargetMuonInOutProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Draw function
  void DrawEvent();
  
protected:

  TargetMuonInOutTracker* fTracker; ///< Pointer to associated tracker SD

  bool fSave; ///< Flag to save event info automatically

  int fMuonInTimeIndex; ///< Time Ntuple Index
  int fMuonInMomXIndex; ///< MomX Ntuple Index
  int fMuonInMomYIndex; ///< MomY Ntuple Index
  int fMuonInMomZIndex; ///< MomZ Ntuple Index
  int fMuonInPosXIndex; ///< PosX Ntuple Index
  int fMuonInPosYIndex; ///< PosY Ntuple Index
  int fMuonInPosZIndex; ///< PosZ Ntuple Index

  int fMuonOutTimeIndex; ///< Time Ntuple Index
  int fMuonOutMomXIndex; ///< MomX Ntuple Index
  int fMuonOutMomYIndex; ///< MomY Ntuple Index
  int fMuonOutMomZIndex; ///< MomZ Ntuple Index
  int fMuonOutPosXIndex; ///< PosX Ntuple Index
  int fMuonOutPosYIndex; ///< PosY Ntuple Index
  int fMuonOutPosZIndex; ///< PosZ Ntuple Index

};


}
#endif


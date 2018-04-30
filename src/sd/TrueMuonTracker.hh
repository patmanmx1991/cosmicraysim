#ifndef __TrueMuonTracker_HH__
#define __TrueMuonTracker_HH__

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
class TrueMuonTracker : public VDetector {
public:

  /// Main constructor from a database table
  TrueMuonTracker(DBTable table);
  /// Simple C++ constructor
  TrueMuonTracker(std::string name, std::string id, bool autoprocess=true, bool autosave=true);
  /// Destructor
  ~TrueMuonTracker(){};

  /// Main processing. Looks for highest momentum track.
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  // Getter Functions
  inline G4double      GetMuonTime(){ return fMuonTime; };
  inline G4ThreeVector  GetMuonMom(){ return fMuonMom;  };
  inline G4ThreeVector  GetMuonPos(){ return fMuonPos;  };
  inline G4double       GetTotEDep(){ return fTotEDep;  };
  inline int            GetMuonPDG(){ return fMuonPDG;  };

protected:
  
  G4double      fMuonTime; ///< HM Muon Step Time
  G4ThreeVector fMuonMom;  ///< HM Muon Mom Vector
  G4ThreeVector fMuonPos;  ///< HM Muon Pos Vector
  G4double      fTotEDep;  ///< Total Energy Deposited this event
  int           fMuonPDG;  ///< Muon PDG Code
  int           fNMuonHits; //< Number of deposits

};

/// True Muon Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class TrueMuonProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  TrueMuonProcessor(TrueMuonTracker* trkr, bool autosave=true);
  /// Destructor
  ~TrueMuonProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Draw function
  void DrawEvent();
  
protected:

  TrueMuonTracker* fTracker; ///< Pointer to associated tracker SD

  bool fSave; ///< Flag to save event info automatically

  int fMuonTimeIndex; ///< Time Ntuple Index
  int fMuonMomXIndex; ///< MomX Ntuple Index
  int fMuonMomYIndex; ///< MomY Ntuple Index
  int fMuonMomZIndex; ///< MomZ Ntuple Index
  int fMuonPosXIndex; ///< PosX Ntuple Index
  int fMuonPosYIndex; ///< PosY Ntuple Index
  int fMuonPosZIndex; ///< PosZ Ntuple Index
  int fMuonPDGIndex;  ///< MPDG Ntuple Index

};


}
#endif


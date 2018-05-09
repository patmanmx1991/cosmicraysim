#ifndef __NeutronSD_HH__
#define __NeutronSD_HH__

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

/// True Neutron Tracker Detector Object :
class NeutronSD : public VDetector {

public:

  /// Main constructor from a database table
  NeutronSD(DBTable table);
  /// Simple C++ constructor
  NeutronSD(std::string name, std::string id, bool autoprocess=true, bool autosave=true);
  /// Destructor
  ~NeutronSD(){};

  /// Main processing
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  // Getter Functions
  inline G4double      GetNeutronTime(){ return fNeutronTime; };
  inline G4ThreeVector  GetNeutronPos(){ return fNeutronPos;  };
  inline G4double       GetTotEDep(){ return fTotEDep;  };
  inline int            GetNeutronPDG(){ return fNeutronPDG;  };
  inline int            GetNeutronHitsNumber(){ return fNNeutronHits;  };

protected:

  G4double      fNeutronTime; ///< HM Neutron Step Time
  G4ThreeVector fNeutronPos;  ///< HM Neutron Pos Vector
  G4double      fTotEDep;  ///< Total Energy Deposited this event
  int           fNeutronPDG;  ///< Neutron PDG Code
  int           fNNeutronHits; //< Number of deposits

};

///  Neutron Processor Object :
/// By default this is created alongside the true Neutron
/// tracker object, so the info is automatically added
/// to the TTree.
class NeutronProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  NeutronProcessor(NeutronSD* trkr, bool autosave=true);
  /// Destructor
  ~NeutronProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Draw function
  void DrawEvent();

protected:

  NeutronSD* fTracker; ///< Pointer to associated tracker SD

  bool fSave; ///< Flag to save event info automatically

  int fNeutronTimeIndex; ///< Time Ntuple Index
  int fNeutronEdepIndex; ///< Edep Ntuple Index
  int fNeutronMultIndex; ///< Multiplicity Ntuple Index
  int fNeutronPosXIndex; ///< PosX Ntuple Index
  int fNeutronPosYIndex; ///< PosY Ntuple Index
  int fNeutronPosZIndex; ///< PosZ Ntuple Index
  int fNeutronPDGIndex;  ///< MPDG Ntuple Index

};


}
#endif

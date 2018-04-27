#ifndef __TrueMuonPoCA_HH__
#define __TrueMuonPoCA_HH__

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
#include "sd/TrueMuonTracker.hh"
#include "db/DBTable.hh"

namespace COSMIC {

/// True Muon Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class TrueMuonPoCAProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  TrueMuonPoCAProcessor(DBTable tbl);

  /// Destructor
  ~TrueMuonPoCAProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

protected:

  TrueMuonTracker* fTrackerA; ///< Pointer to associated tracker A SD
  TrueMuonTracker* fTrackerB; ///< Pointer to associated tracker B SD

  bool fSave; ///< Flag to save event info automatically

  int fPoCATimeIndex; ///< Time Ntuple Index
  int fPoCAPosXIndex; ///< PosX Ntuple Index
  int fPoCAPosYIndex; ///< PosY Ntuple Index
  int fPoCAPosZIndex; ///< PosZ Ntuple Index
  int fPoCADistIndex; ///< Distance at PoCA
  int fPoCAAnglIndex; ///< Angle at PoCA

};

}
#endif

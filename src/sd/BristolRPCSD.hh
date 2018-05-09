#ifndef __BristolRPCSD_HH__
#define __BristolRPCSD_HH__

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
#include "DriftHit.hh"

namespace COSMIC {

// Forward Declarations
class DBTable;

/// True Muon Tracker Detector Object :
/// Saves the highest momentrum muon track
/// that entered a detector logical voluem
class BristolRPCSD : public VDetector {
public:

  /// Main constructor from a database table
  BristolRPCSD(DBTable table);
  /// Simple C++ constructor
  BristolRPCSD(std::string name, std::string id, bool autoprocess = true, bool autosave = true);
  /// Destructor
  ~BristolRPCSD() {};

  /// Main processing. Looks for highest momentum track.
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Initialize the Hit Collection
  void Initialize(G4HCofThisEvent*HCE);

  /// Getter functions
  inline DriftChamberHitsCollection* GetHC() { return fHitsCollection; };
  G4int GetHCID();
  G4int GetGhostHCID();

  void SetLogicalVolume(G4LogicalVolume* logic, G4VPhysicalVolume* physical);

  double GetEfficiency() { return fEfficiency; };

  /// Draw the hits
  void DrawAll();

  void ResetState();

protected:

  DriftChamberHitsCollection* fHitsCollection;
  G4int fHCID;

  G4LogicalVolume* fLogical;
  G4VPhysicalVolume* fPhysical;

  G4double fResolution;

  G4double fDetectorSizeX;
  G4double fDetectorSizeY;
  G4double fDetectorSizeZ;

  G4double fEfficiency;
  G4double fEfficiencyThrow;
};

/// LongDrift Processor Object :
/// By default this is created alongside the true muon
/// tracker object, so the info is automatically added
/// to the TTree.
class BristolRPCProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  BristolRPCProcessor(BristolRPCSD* trkr, bool autosave = true);
  /// Destructor
  ~BristolRPCProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Check if info should be saved and do so, return true if info valid
  bool FillNTuples();

  /// Reset all information
  void Reset();

  // Getter Functions
  inline G4double       GetTime() { return fTime; };

  inline G4double       GetPosX() { return fPosX;  };
  inline G4double       GetPosY() { return fPosY;  };
  inline G4double       GetPosZ() { return fPosZ;  };
  inline G4double       GetErrX() { return fErrX;  };
  inline G4double       GetErrY() { return fErrY;  };
  inline G4double       GetErrZ() { return fErrZ;  };

  inline G4double       GetWorldPosX() { return fWorldPosX;  };
  inline G4double       GetWorldPosY() { return fWorldPosY;  };
  inline G4double       GetWorldPosZ() { return fWorldPosZ;  };
  inline G4double       GetWorldErrX() { return fWorldErrX;  };
  inline G4double       GetWorldErrY() { return fWorldErrY;  };
  inline G4double       GetWorldErrZ() { return fWorldErrZ;  };

  /// Assign the detector to a logical volume
  void SetLogicalVolume(G4LogicalVolume* logic, G4VPhysicalVolume* physical);

protected:

  BristolRPCSD* fDetector; ///< Pointer to associated to SD
  G4int fHCID;

  G4LogicalVolume* fLogical;
  G4VPhysicalVolume* fPhysical;

  bool fSave; ///< Flag to save event info automatically

  G4double fTime;

  G4double fPosX;
  G4double fPosY;
  G4double fPosZ;
  G4double fErrX;
  G4double fErrY;
  G4double fErrZ;

  G4double fWorldPosX;
  G4double fWorldPosY;
  G4double fWorldPosZ;
  G4double fWorldErrX;
  G4double fWorldErrY;
  G4double fWorldErrZ;

  int fTimeIndex;

  int fPosXIndex;
  int fPosYIndex;
  int fPosZIndex;
  int fErrXIndex;
  int fErrYIndex;
  int fErrZIndex;

  int fWorldPosXIndex;
  int fWorldPosYIndex;
  int fWorldPosZIndex;
  int fWorldErrXIndex;
  int fWorldErrYIndex;
  int fWorldErrZIndex;
};


}
#endif






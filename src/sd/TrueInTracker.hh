#ifndef __TrueInAbsorber_HH__
#define __TrueInAbsorber_HH__

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

/// True In Tracker SD. Keeps Track of all incoming particles into a SD
class TrueInAbsorber : public VDetector {
public:

  /// Main constructor from a database table
  TrueInAbsorber(DBTable table);
  /// Simple C++ constructor
  TrueInAbsorber(std::string name, std::string id, bool autoprocess=true, bool autosave=true);
  /// Destructor
  ~TrueInAbsorber(){};

  /// Main processing. Looks for highest momentum track.
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);

  /// Reset all information
  void ResetState();

  // Get Functions
  inline int GetNParticles()  { return particle_pdg.size(); };

  inline std::vector<int>&     GetParticlePDG() { return particle_pdg; };
  inline std::vector<double>&  GetParticleT()   { return particle_t;   };
  inline std::vector<double>&  GetParticleE()   { return particle_E;   };
  inline std::vector<double>&  GetParticlePX()  { return particle_px;  };
  inline std::vector<double>&  GetParticlePY()  { return particle_py;  };
  inline std::vector<double>&  GetParticlePZ()  { return particle_pz;  };
  inline std::vector<double>&  GetParticleX()   { return particle_x;   };
  inline std::vector<double>&  GetParticleY()   { return particle_y;   };
  inline std::vector<double>&  GetParticleZ()   { return particle_z;   };

protected:
  
  std::vector<int>     particle_pdg;

  std::vector<double>  particle_t;  
  std::vector<double>  particle_E;

  std::vector<double>  particle_px;
  std::vector<double>  particle_py;
  std::vector<double>  particle_pz;

  std::vector<double>  particle_x;
  std::vector<double>  particle_y;
  std::vector<double>  particle_z;


};

/// True In Tracker Processor. Registers vectors to ttree
class TrueInProcessor : public VProcessor {
public:

  /// Processor can only be created with an associated
  /// tracker object.
  TrueInProcessor(TrueInAbsorber* trkr, bool autosave=true);

  /// Destructor
  ~TrueInProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);
  
protected:

  TrueInAbsorber* fTracker; ///< Pointer to associated tracker SD
  bool fSave;

};

} // - namespace COSMIC
#endif


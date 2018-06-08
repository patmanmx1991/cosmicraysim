#ifdef __USE_PUMAS__
//******************************************************************************
// PumasBackwardsGenerator.hh
//
// This class is a class derived from G4VUserPumasBackwardsGenerator for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef __COSMIC_PumasBackwardsGenerator_HH__
#define __COSMIC_PumasBackwardsGenerator_HH__ 1

#include <iomanip>
#include "vector"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "G4Types.hh"
#include <G4Box.hh>

#include "analysis/Analysis.hh"
#include "analysis/VFluxProcessor.hh"

#include "db/DB.hh"


#include "PumasBackwardsGenerator.hh"

// Forward Declarations
class G4Event;

namespace COSMIC {


//---------------------------------------------------------------------------------
/// Shukla Flux Generator, derived from CRESTA code written by Chris Steer.
class PumasBackwardsGenerator : public G4VUserPrimaryGeneratorAction
{
public:

    /// Constructor
    PumasBackwardsGenerator();
    /// Destructor
    ~PumasBackwardsGenerator();

    void Setup();

    /// Generate the primary muon, and apply acceptance filter.
    void GeneratePrimaries(G4Event* anEvent);

    bool fSetupState;

    // Get Functions
    inline G4double GetMuonTime()     { return fMuonTime;     };
    inline G4double GetMuonEnergy()   { return fMuonEnergy;   };
    inline G4ThreeVector GetMuonDir() { return fMuonDir;      };
    inline G4ThreeVector GetMuonPos() { return fMuonPos;      };
    inline int GetMuonPDG()           { return fMuonPDG;      };
    inline G4double GetExposureTime() { return fExposureTime; };
    inline void SetExposureTime(double d){ fMuonTime = d; };

private:

    G4double fMinEnergy; ///< Min Energy Range to integrate/throw
    G4double fMaxEnergy; ///< Max Energy Range to integrate/throw

    TF1 *fEnergyPDF; ///< Energy Function from Shukla Paper
    TF1 *fZenithPDF; ///< Zenith Function from Shukla Paper

    G4double fFluxIntegrated; ///< Integral of flux for norm.

    G4double fPar_I0;  ///< IO  Par from Shukla Paper
    G4double fPar_n;   ///< n   Par from Shukla Paper
    G4double fPar_E0;  ///< E0  Par from Shukla Paper
    G4double fPar_eps; ///< eps Par from Shukla Paper

    G4double fPar_rad; ///< radius Par from Shukla Paper
    G4double fPar_dis; ///< distance Par from Shukla Paper

    /// Definitions for particle gun to avoid string comparisons
    std::vector<G4ParticleDefinition*> fParticleDefs;
    G4ParticleGun* fParticleGun; ///< Main particle gun

    // Source box setup originally used geometries from the main GEO
    // volume list, but no longer does. That is why it is setup this
    // way.
    bool fSourceBox; ///< FLAG : Whether source box been created/checked
    G4ThreeVector fSourceBoxWidth; ///< Length/Width of box.
    G4ThreeVector fSourceBoxPosition; ///< Position of box in world volume
    G4int fSourceBoxRequireN; ///< Requires at least this number of hits in different target boxes

    G4double fArea; ///< Area of box. Used for normalisation.

    // Target box also originally setup from GEO tables.
    bool fCheckTargetBoxes; ///< FLAG : Whether target boxes okay.
    std::vector<G4Box*> fTargetBoxes; ///< Geant4 Box Object for each target
    std::vector<G4ThreeVector> fTargetBoxPositions; ///< Position in world volume for each target
    // G4int fTargetBoxesRequireN; ///< Requires at least this number of hits in different target boxes

    // Throws are tracked regardless of acceptance, so integrated time always correct.
    /// Current Integrated Exposure time. Derivide from NThrows and Integrated flux.
    G4double fExposureTime;
    int fNThrows; ///< Number of throws ran so far.


    G4double fMuonTime;     ///< MuonTime   Info for auto flux processor
    G4double fMuonEnergy;   ///< MuonEnergy Info for auto flux processor
    G4ThreeVector fMuonDir; ///< MuonDir    Info for auto flux processor
    G4ThreeVector fMuonPos; ///< MuonPos    Info for auto flux processor
    G4double fMuonPDG;      ///< MuonPDG    Info for auto flux processor
    G4double fSpeedUp;      ///< MuonPDG    Info for auto flux processor

    double fKineticThreshold;

};
//---------------------------------------------------------------------------------




//---------------------------------------------------------------------------------
/// CRYFluxProcessor class : Automatically saves the true muon information
/// for each event into the TTree. Currently only saves the total exposure
/// since CRY energy can save multiple primaries.
class PumasBackwardsProcessor : public VFluxProcessor {
public:
  /// Processor can only be created with an associated
  /// tracker object.
  PumasBackwardsProcessor(PumasBackwardsGenerator* gen, bool autosave = true);
  /// Destructor
  ~PumasBackwardsProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Save the information the generator recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Return an integrated exposure time in s. Used for
  /// ending the run after so many seconds.
  G4double GetExposureTime() { return 1.0; };

  void ResetExposureTime(){};
  double GetEventRate(){ return 1.0; };

protected:

  PumasBackwardsGenerator* fGenerator; ///< Pointer to associated generator
  

};
//---------------------------------------------------------------------------------
#endif __USE_PUMAS__
} // - namespace COSMIC
#endif

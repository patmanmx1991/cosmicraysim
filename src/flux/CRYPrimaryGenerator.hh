//******************************************************************************
// CRYPrimaryGenerator.hh
//
// This class is a class derived from G4VUserCRYPrimaryGenerator for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef __COSMIC_CRYPrimaryGenerator_HH__
#define __COSMIC_CRYPrimaryGenerator_HH__ 1

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
#include "db/DBLink.hh"

#include "cry/RNGWrapper.hh"
#include "cry/CRYSetup.h"
#include "cry/CRYGenerator.h"
#include "cry/CRYParticle.h"
#include "cry/CRYUtils.h"

#include "CRYPrimaryGenerator.hh"

// Forward Declarations
class G4Event;

namespace COSMIC {

//---------------------------------------------------------------------------------
/// CRY Flux Generator, derived from example given in CRY Library
class CRYPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:

  /// Constructor
  CRYPrimaryGenerator();
  /// Destructor
  ~CRYPrimaryGenerator();

  /// Produce a new CRY generator given the current setup
  void UpdateCRY();

  /// Return the source box used
  G4Box* GetSourceBox();

  /// Return the target boxes list
  std::vector<G4Box*> GetTargetBoxes();

  /// Return the Target Box Positions List
  std::vector<G4ThreeVector> GetTargetBoxPositions();

  /// Function to draw events from CRY library
  void GeneratePrimaries(G4Event* anEvent);

  // Getter Functions
  G4double GetTime() {return fExposureTime;};

private:

  CRYGenerator* gen; ///< Actual CRY Generator

  std::vector<CRYParticle*> *vect; ///< vector of generated particles

  G4ParticleTable* particleTable; ///< Available of primaries table
  G4ParticleGun* particleGun;     ///< Actual G4 Gun

  G4int InputState; ///< Flag to check CRY setup

  // CRY Settings
  bool fGenNeutrons  ; ///< Flag to generate neutrons : Def(true)
  bool fGenProtons   ; ///< Flag to generate protons  : Def(true)
  bool fGenGammas    ; ///< Flag to generate gammas   : Def(true)
  bool fGenElectrons ; ///< Flag to generate electrons: Def(true)
  bool fGenMuons     ; ///< Flag to generate muons    : Def(true)
  bool fGenPions     ; ///< Flag to generate pions    : Def(true)

  G4double fAltitude; ///< Altitude value for CRY
  G4double fLatitude; ///< Latitude value for CRY [-90,90]

  std::string fDate; ///< CRY date ID : Def("1-1-2007")

  G4int fNParticlesMin; ///< Particle multiplicity truncation
  G4int fNParticlesMax; ///< Particle multiplicity truncation

  std::string fDataDirectory; ///< Default directory of CRY data files


  // Source box setup originally used geometries from the main GEO
  // volume list, but no longer does. That is why it is setup this
  // way.
  G4Box* fSourceBox; ///< Pointer to user setup Source Box
  G4ThreeVector fSourceBoxPosition; ///< Position of source box in world
  G4double fLateralBoxSize; ///< Side length of source box

  // Target box also originally setup from GEO tables.
  bool fCheckTargetBoxes; ///< Flag to check target boxes created
  std::vector<G4Box*> fTargetBoxes; ///< List of user defined targets
  std::vector<G4ThreeVector> fTargetBoxPositions; ///< Position of user defined targets in world


  G4double fExposureTime; ///< Current integrated exposure time
  int fNthrows; ///< Total number of throws so far

};
//---------------------------------------------------------------------------------

//---------------------------------------------------------------------------------
/// CRYFluxProcessor class : Automatically saves the true muon information
/// for each event into the TTree. Currently only saves the total exposure
/// since CRY energy can save multiple primaries.
class CRYPrimaryFluxProcessor : public VFluxProcessor {
public:
  /// Processor can only be created with an associated
  /// tracker object.
  CRYPrimaryFluxProcessor(CRYPrimaryGenerator* gen, bool autosave = true);
  /// Destructor
  ~CRYPrimaryFluxProcessor() {};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Save the information the generator recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Return an integrated exposure time in s. Used for
  /// ending the run after so many seconds.
  G4double GetExposureTime();

protected:

  CRYPrimaryGenerator* fGenerator; ///< Pointer to associated generator
  bool fSave; ///< Flag to save event info automatically
  int fTimeIndex; ///< Time Ntuple Index

};
//---------------------------------------------------------------------------------

} // - namespace COSMIC
#endif

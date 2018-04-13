//******************************************************************************
// CRYPrimaryGenerator.hh
//
// This class is a class derived from G4VUserCRYPrimaryGenerator for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef CRYPrimaryGenerator_h
#define CRYPrimaryGenerator_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "CRYSetup.h"
#include "CRYGenerator.h"
#include "CRYParticle.h"
#include "CRYUtils.h"
#include "vector"
#include "cry/RNGWrapper.hh"
#include "G4SystemOfUnits.hh"
#include "db/DB.hh"
#include "db/DBLink.hh"

#include "G4ThreeVector.hh"
#include "G4DataVector.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleGun.hh"
#include "Randomize.hh"
#include "globals.hh"
#include "cry/CRYSetup.h"
#include "cry/CRYGenerator.h"
#include "cry/CRYParticle.h"
#include "cry/CRYUtils.h"
#include "vector"
// #include "RNGWrapper.hh"
#include "G4Types.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include <G4Box.hh>
#include "analysis/Analysis.hh"
#include "analysis/VFluxProcessor.hh"

class G4Event;

namespace COSMIC {

class CRYPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    CRYPrimaryGenerator();
    ~CRYPrimaryGenerator();

public:
    void GeneratePrimaries(G4Event* anEvent);
    void InputCRY();
    void UpdateCRY();
        G4Box* GetSourceBox();
    std::vector<G4Box*> GetTargetBoxes();
    std::vector<G4ThreeVector> GetTargetBoxPositions();
    G4double GetTime() {return fExposureTime;};

private:
    std::vector<CRYParticle*> *vect; // vector of generated particles
    G4ParticleTable* particleTable;
    G4ParticleGun* particleGun;
    CRYGenerator* gen;
    G4int InputState;
    DBLink* fTable;

    // Setup Defaults
    bool fGenNeutrons  ;
    bool fGenProtons   ;
    bool fGenGammas    ;
    bool fGenElectrons ;
    bool fGenMuons     ;
    bool fGenPions     ;

    G4double fAltitude; // Sea Level
    G4double fLatitude; // Equator

    std::string fDate = "1-1-2007"; // CRY Default

    G4int fNParticlesMin; // No Truncation
    G4int fNParticlesMax; // No Truncation

    G4double fLateralBoxSize; // This is tricker, define from fSourceBox.

    std::string fDataDirectory;
    G4Box* fSourceBox;
    std::vector<G4Box*> fTargetBoxes;
    std::vector<G4ThreeVector> fTargetBoxPositions;

    
    std::string fSourceBoxIndex;
    bool fCheckTargetBoxes;
    G4double fExposureTime;
    int fNthrows;

    G4ThreeVector fSourceBoxPosition;
};


class CRYPrimaryFluxProcessor : public VFluxProcessor {
public:
  /// Processor can only be created with an associated
  /// tracker object.
  CRYPrimaryFluxProcessor(CRYPrimaryGenerator* gen, bool autosave=true);
  /// Destructor
  ~CRYPrimaryFluxProcessor(){};

  /// Setup Ntuple entries
  bool BeginOfRunAction(const G4Run* run);

  /// Process the information the tracker recieved for this event
  bool ProcessEvent(const G4Event* event);

  /// Return an integrated exposure time in s. Used for
  /// ending the run after so many seconds.
  G4double GetExposureTime();

protected:

  CRYPrimaryGenerator* fGenerator; ///< Pointer to associated tracker SD

  bool fSave; ///< Flag to save event info automatically

  int fTimeIndex; ///< Time Ntuple Index


};

}
#endif

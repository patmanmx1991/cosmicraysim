//******************************************************************************
// PrimaryGeneratorAction.hh
//
// This class is a class derived from G4VUserPrimaryGeneratorAction for
// constructing the process used to generate incident particles.
//
// 1.00 JMV, LLNL, JAN-2007:  First version.
//******************************************************************************
//
#ifndef __ShuklaPrimaryGenerator_H__
#define __ShuklaPrimaryGenerator_H__

#include "TF1.h"
#include <G4Box.hh>

#include "G4VUserPrimaryGeneratorAction.hh"

// #include "PrimaryGeneratorMessenger.hh"

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
// #include "RNGWrapper.hh"
#include "G4Types.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "db/DBLink.hh"
#include "geo/simple/GeoBox.hh"

#include "G4Event.hh"

#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"

#include "TF1.h"
#include "TMath.h"
#include <G4Box.hh>
#include <cmath>
#include <iomanip>
#include "analysis/Analysis.hh"
#include "analysis/VFluxProcessor.hh"

namespace COSMIC {
namespace { G4Mutex myMutex = G4MUTEX_INITIALIZER; }

class ShuklaPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    ShuklaPrimaryGenerator();
    ~ShuklaPrimaryGenerator();

    void SampleParticleType();
    G4ThreeVector SampleDirection();
    void GeneratePrimaries(G4Event* anEvent);

    void GetSourceBox();
    std::vector<G4Box*> GetTargetBoxes();
    std::vector<G4ThreeVector> GetTargetBoxPositions();

    G4double GetMuonTime() {return fMuonTime;};
    G4double GetMuonEnergy() {return fMuonEnergy;};
    G4ThreeVector GetMuonDir() { return fMuonDir;};
    G4ThreeVector GetMuonPos() { return fMuonPos;};
    int GetMuonPDG() { return fMuonPDG; };

    inline G4double GetExposureTime() {return fExposureTime;};

private:

    DBLink* fTable;

    G4double fMinEnergy;
    G4double fMaxEnergy;

    TF1 *fEnergyPDF;
    TF1 *fZenithPDF;

    G4double fFluxIntegrated;

    G4double fPar_I0;
    G4double fPar_n;
    G4double fPar_E0;
    G4double fPar_eps;
    G4double fPar_rad;
    G4double fPar_dis;

    G4double fMuonTime;
    G4double fMuonEnergy;
    G4ThreeVector fMuonDir;
    G4ThreeVector fMuonPos;
    G4double fMuonPDG;

    std::vector<G4ParticleDefinition*> fParticleDefs;
    G4ParticleGun* fParticleGun;

    bool fSourceBox;
    G4ThreeVector fSourceBoxWidth;
    std::vector<G4Box*> fTargetBoxes;
    std::vector<G4ThreeVector> fTargetBoxPositions;

    std::string fSourceBoxIndex;
    bool fCheckTargetBoxes;

    G4ThreeVector fSourceBoxPosition;

    G4double fExposureTime;
    int fNThrows;
    G4double fArea;

};

class ShuklaPrimaryFluxProcessor : public VFluxProcessor {
public:
    /// Processor can only be created with an associated
    /// tracker object.
    ShuklaPrimaryFluxProcessor(ShuklaPrimaryGenerator* gen, bool autosave = true);
    /// Destructor
    ~ShuklaPrimaryFluxProcessor() {};

    /// Setup Ntuple entries
    bool BeginOfRunAction(const G4Run* run);

    /// Process the information the tracker recieved for this event
    bool ProcessEvent(const G4Event* event);

    /// Return an integrated exposure time in s. Used for
    /// ending the run after so many seconds.
    G4double GetExposureTime();

protected:

    ShuklaPrimaryGenerator* fGenerator; ///< Pointer to associated tracker SD

    bool fSave; ///< Flag to save event info automatically

    int fMuonTimeIndex; ///< Time Ntuple Index
    int fMuonEnergyIndex; ///< Energy Ntuple Index
    int fMuonDirXIndex; ///< DirX Ntuple Index
    int fMuonDirYIndex; ///< DirY Ntuple Index
    int fMuonDirZIndex; ///< DirZ Ntuple Index
    int fMuonPosXIndex; ///< PosX Ntuple Index
    int fMuonPosYIndex; ///< PosY Ntuple Index
    int fMuonPosZIndex; ///< PosZ Ntuple Index
    int fMuonPDGIndex;  ///< MPDG Ntuple Index

};



} // - namespace COSMIC

#endif

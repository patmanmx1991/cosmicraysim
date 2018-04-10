#ifndef __TrueMuonTracker_HH__
#define __TrueMuonTracker_HH__

#include "DetectorObject.hh"
#include "G4UserRunAction.hh"
#include "globals.hh"

#include "G4Run.hh"
#include "globals.hh"
#include "g4root.hh"


namespace COSMIC {


class TrueParticleTracker : public DetectorObject {
public:
  TrueParticleTracker(DBLink* table);
  ~TrueParticleTracker();

  bool BeginOfRunAction(const G4Run* run);  
  bool RecordEvent(const G4Event* event);
  G4bool ProcessHits(G4Step*, G4TouchableHistory*);
  void Reset();

  DBLink* fTable;

  std::string fLogicalName;

  std::string fType;
  std::string fTableIndex;
  std::string fLogical;

  int fIndexEnergy;

  G4double      fMuonTime;
  G4ThreeVector fMuonMom;
  G4ThreeVector fMuonPos;
  G4double      fMuonMomMag2;
  G4double      fTotEDep;
  int fMuonPDG;

  int fMuonTimeIndex;
  int fMuonMomXIndex;
  int fMuonMomYIndex;
  int fMuonMomZIndex;
  int fMuonPosXIndex;
  int fMuonPosYIndex;
  int fMuonPosZIndex;
  int fMuonPDGIndex;
};

}
#endif


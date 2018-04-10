#ifndef __DetectorObject_HH_
#define __DetectorObject_HH_

#include "B3Hits.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"
#include "db/DBLink.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4THitsMap.hh"
#include "G4SystemOfUnits.hh"

#include "geo/GeoUtils.hh"

class DetectorConstruction;
class G4Step;
class G4HCofThisEvent;

namespace COSMIC{

class DetectorObject : public G4VSensitiveDetector
{
public:
  DetectorObject(DBLink* table);
  ~DetectorObject();

  // virtual void Construct() = 0;
  
  // virtual int GetCollectionID() = 0;

  virtual bool RecordEvent(const G4Event* event) = 0;
  virtual bool BeginOfRunAction(const G4Run* run) = 0;
  virtual G4bool ProcessHits(G4Step*, G4TouchableHistory*) = 0;
  
  DBLink* fTable;

  std::string fLogicalName;

  std::string fType;
  std::string fTableIndex;
};
}

#endif

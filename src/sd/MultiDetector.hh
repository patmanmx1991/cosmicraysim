#ifndef __MULTIDETECTOR_HH__
#define __MULTIDETECTOR_HH__

#include "DetectorObject.hh"
#include "G4UserRunAction.hh"
#include "globals.hh"

#include "G4Run.hh"
#include "globals.hh"
#include "g4root.hh"


namespace COSMIC {

class MultiDetector : public G4MultiFunctionalDetector {
public:
  MultiDetector(DBLink* table);
  ~MultiDetector();

  int GetCollectionID();

  bool BeginOfRunAction(const G4Run* run);  
  bool RecordEvent(const G4Event* event);

  int fCollID;

  DBLink* fTable;

  std::string fLogicalName;

  std::string fType;
  std::string fTableIndex;
  std::string fLogical;

  int fIndexEnergy;

};
}
#endif


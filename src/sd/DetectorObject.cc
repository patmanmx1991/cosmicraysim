#include "DetectorObject.hh"

#include "G4AnalysisManagerState.hh"
#include "g4root.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

namespace  COSMIC{

DetectorObject::DetectorObject(DBLink* table)
: G4VSensitiveDetector(table->GetIndexName()){
  fTable = table;
}

DetectorObject::~DetectorObject(){
}

} // - namespace COSMIC

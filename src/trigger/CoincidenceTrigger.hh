#ifndef __COINC_TRIGGER_HH__
#define __COINC_TRIGGER_HH__

#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

#include "analysis/VTrigger.hh"

namespace COSMIC {

// Forward declarations
class DBTable;
class VProcessor;

/// Simple co-incident trigger class, with
/// a single optional energy and time threshold.
class CoincidenceTrigger : public VTrigger {
public:

  /// Database constructor
  CoincidenceTrigger(DBTable tbl);

  /// Simple Generic C++ Constructor
  CoincidenceTrigger(std::string name,
                std::vector<std::string> processors,
                G4int hit_detectors,
                G4double ethresh);

  /// Destructor
  ~CoincidenceTrigger() {};

  /// Function to setup detector processors from global list
  void SetupProcessors(std::vector<std::string> det);

  /// Trigger Processing
  bool ProcessTrigger(const G4Event* event);

protected:

  std::vector<VProcessor*> fProcessors; ///< Vector of all valid processors
  G4int fRequireN;// Requires N detectors to be hit in order to trigger
  G4double fEnergyThreshold; ///< Energy Threshold setting, applied to all processors

};

} // - namespace COSMIC
#endif

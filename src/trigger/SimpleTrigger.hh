#ifndef __SIMPLE_TRIGGER_HH__
#define __SIMPLE_TRIGGER_HH__

#include "G4SystemOfUnits.hh"
#include "G4Event.hh"

#include "analysis/VTrigger.hh"

namespace COSMIC {

// Forward declarations
class DBTable;
class VProcessor;

/// Simple co-incident trigger class, with
/// a single optional energy and time threshold.
class SimpleTrigger : public VTrigger {
public:

  /// Database constructor
  SimpleTrigger(DBTable tbl);

  /// Simple Generic C++ Constructor
  SimpleTrigger(std::string name,
                std::vector<std::string> processors,
                G4double ethresh = -1.0,
                G4double tthresh = -1.0);

  /// Destructor
  ~SimpleTrigger() {};

  /// Function to setup detector processors from global list
  void SetupProcessors(std::vector<std::string> det);

  /// Trigger Processing
  bool ProcessTrigger(const G4Event* event);

protected:
  std::vector<VProcessor*> fProcessors; ///< Vector of all valid processors
  G4double fEnergyThreshold; ///< Energy Threshold setting, applied to all processors
  G4double fTimeThreshold; ///< Maximum time window between active processors
};

} // - namespace COSMIC
#endif

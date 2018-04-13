#ifndef __COSMIC_VTRIGGER_HH__
#define __COSMIC_VTRIGGER_HH__

#include <iostream>
#include "G4Event.hh"
#include "G4Run.hh"

namespace COSMIC{

/// Virtual Trigger template class
/// Classes that inherit from this should
/// access the available processors in the 
/// main analysis manager, or access event 
/// information directly to determine if an
/// event is signal or not.
class VTrigger {
public:
  
  /// Emptry Constructor
  VTrigger(){};
  /// Named Constructor
  VTrigger(std::string id) : fID(id) {};
  /// Destructor
  ~VTrigger(){};

  /// User should overwrite this function to return
  /// true when an event is triggered. The processor
  /// stage occurs before triggers are called, so
  /// this function can also use information from any
  /// loaded processors.
  virtual bool ProcessTrigger(const G4Event* event) = 0;

  /// Reset function to reset trigger state at start of event
  virtual inline void Reset(){ fTriggered = false; };
  
    /// Set Trigger ID
  virtual inline void SetID(std::string id){ fID = id; };

  /// Return Trigger ID
  virtual inline std::string GetID(){ return fID; };

  /// Return trigger, incase it is needed later
  virtual inline bool GetTrigger(){ return fTriggered; };

protected:
  bool fTriggered; ///< Current state of this trigger
  std::string fID; ///< ID of this trigger
};

} // - namespace COSMIC
#endif

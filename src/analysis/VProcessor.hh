#ifndef __COSMIC_VPROCESSOR_HH__
#define __COSMIC_VPROCESSOR_HH__

#include "G4SystemOfUnits.hh"
#include "G4Event.hh"
#include "G4Run.hh"

namespace COSMIC {

/// Virtual Processor Template Class.
/// Used as a template for processors that convert
/// information from run information, e.g. detector hits,
/// into useful information and save it to the outputs.
class VProcessor {
public:
  /// Empty Constructor
  VProcessor(){};
  /// Named Constructor
  VProcessor(std::string id) : fID(id) {};
  /// Destructor
  ~VProcessor(){};

  /// Virtual start of run action. All processors
  /// should implement this, as it is used to setup
  /// Ntuple arrays.
  virtual bool BeginOfRunAction(const G4Run* run) = 0;

  /// Virtual Event Processing action. All processors
  /// should implement this, as it is used to extract
  /// information from the event.
  virtual bool ProcessEvent(const G4Event* event) = 0;

  /// Virtual reset function, used to reset processor
  /// information at the start of an event.
  virtual inline void Reset() {
    fHasInfo = false;
    fTime      = -999.9;
    fEnergy    = -999.9;
  }

  /// Set Processor ID
  virtual inline void SetID(std::string id){ fID = id; };

  /// Return Processor ID
  virtual inline std::string GetID(){ return fID; };

  /// Return whether the processor has any valid info.
  virtual inline bool HasInfo() { return fHasInfo; };

  /// Return a processed time if available. Used for
  /// simple co-incident triggers.
  virtual inline G4double GetTime() { return fTime; };

  /// Return a processed energy if available. Used for
  /// simple co-incident triggers.
  virtual inline G4double GetEnergy() { return fEnergy; };

protected:

  bool fHasInfo; ///< Whether the event had anything useful in it.
  G4double fTime; ///< Average hit time  that can be set by the user.
  G4double fEnergy; ///< Average energy deposited that can be set by the user.
  std::string fID; ///< ID of this processor

};

} // - namespace COSMIC
#endif

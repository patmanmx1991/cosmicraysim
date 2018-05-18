#ifndef __COSMIC_VDETECTOR_HH_
#define __COSMIC_VDETECTOR_HH_

#include "G4VSensitiveDetector.hh"
#include "G4Event.hh"

namespace COSMIC {

/// Template detector class for use in the analysis.
/// Uses most of the sensitive detector functionality,
/// but has a ResetState function to restore all variables
/// at the start of an event.
class VDetector : public G4VSensitiveDetector
{
public:

  /// Set ID on construction
  inline VDetector(std::string id, std::string type) :
    G4VSensitiveDetector(id) {
    fID = id;
    fType = type;
  };

  /// Destructor
  virtual ~VDetector() {};


  /// Reset detector state at start of event
  virtual void ResetState() {};


  /// Assign the detector to a logical volume
  virtual inline void SetLogicalVolume(G4LogicalVolume* logic, G4VPhysicalVolume* /*physical*/) {
    logic->SetSensitiveDetector(this);
  }


  /// Set this detectors ID tag
  inline void SetID(std::string id) {fID = id;};
  /// Get this detectors ID tag
  inline std::string GetID() {return fID;};


  /// Set this detectors type string for reference
  inline void SetType(std::string type) {fType = type;};
  /// Get this detectors type string for reference
  inline std::string GetType() {return fType;};


protected:
  std::string fType; ///< Detector type
  std::string fID;   ///< Detector Unique ID tag
};
}

#endif


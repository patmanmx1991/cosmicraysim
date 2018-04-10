#ifndef __DETECTOR_MANAGER_HH__
#define __DETECTOR_MANAGER_HH__

#include <map>

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4GlobalMagFieldMessenger.hh"

#include "geo/GeoObject.hh"
#include "db/DBLink.hh"
#include "sd/DetectorObject.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"

namespace COSMIC {

class DetectorManager {
public:
  DetectorManager();
  ~DetectorManager() {};

  // void ConstructAll();
  void RegisterSensitiveDetector(std::string name, G4VSensitiveDetector* sd);
  inline int GetNDetectors() { return fDetectors.size(); };
  inline std::vector<G4VSensitiveDetector*> GetDetectors() { return fDetectors; }
  bool BeginOfRunAction(const G4Run* run, int& ntupleid, int& histid);
  bool RecordEvent(const G4Event* event);

  // Static singleton get method
  static inline DetectorManager *Get()
  { return fPrimary == 0 ? fPrimary = new DetectorManager : fPrimary; };
  static DetectorManager *fPrimary;

protected:
  
  // std::vector<DBLink*> fDetTables;

  std::vector<G4VSensitiveDetector*> fDetectors;
  std::vector<std::string> fNames;
  std::vector<bool> fRegistered;

};

namespace DetectorObjectFactory {
  // Function to create detector objects from tables
  G4VSensitiveDetector* Construct(DBLink* table);
  G4VSensitiveDetector* CreateSD(std::string sensitive);

} // - namespace DetectorObjectFactory

} // - namespace COSMIC
#endif

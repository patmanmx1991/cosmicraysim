#ifndef __DET_BUILDER_HH__
#define __DET_BUILDER_HH__

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"

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

///Task5d.1: Don't forget to include header file of your sensitive detector
#include "db/DB.hh"
#include "db/DBTable.hh"
#include <map>

//$$$$$$$$$$$$$$$$$$ task 1c
#include "G4GlobalMagFieldMessenger.hh"
//$$$$$$$$$$$$$$$$$$$$$$$$$$
#include "GeoObject.hh"

namespace COSMIC {

class GeoManager {
public:
  GeoManager();
  ~GeoManager() {};

  G4LogicalVolume* GetMother(DBLink* geo_tab);

  G4VPhysicalVolume* ConstructAll();
  std::vector<DBLink*> fGeoTables;
  std::map<std::string, GeoObject*> fGeoObjects;
  int MotherStatus(DBLink* geo_tab);
  bool HasGeoObject(std::string name);

  // void ConstructSensitive();
  std::vector<std::string> fGeoIDs;
  static inline GeoManager *Get()
  { return fPrimary == 0 ? fPrimary = new GeoManager : fPrimary; };
  static GeoManager *fPrimary;

};


namespace GeoObjectFactory {
GeoObject* Construct(DBLink* table);
}

}
#endif


#include "DetectorConstruction.hh"

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
#include <map>
#include "geo/GeoManager.hh"
#include "G4VUserDetectorConstruction.hh"


#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoManager.hh"
#include "sd/DetectorManager.hh"
#include "geo/GeoManager.hh"
#include "trigger/TriggerManager.hh"
#include "processors/ProcessorFactory.hh"
#include "analysis/Analysis.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "DetectorMessenger.hh"
#include "G4RunManager.hh"

using namespace COSMIC;

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction()
{
  DefineMaterials();
  //fMessenger = new DetectorMessenger(this);
}

DetectorConstruction::~DetectorConstruction()
{}

void DetectorConstruction::DefineMaterials()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Clear any old processors/triggers 
  //  G4GeometryManager::GetInstance()->OpenGeometry();
  //  G4PhysicalVolumeStore::GetInstance()->Clean();
  //  G4LogicalVolumeStore::GetInstance()->Clean();
  //  G4SolidStore::GetInstance()->Clean();
  //  Analysis::Get()->DestroyTriggers();
  
  // Build entire geometry
  G4VPhysicalVolume* world = GeoManager::Get()->ConstructAll();

  // Create the triggers/processors
  //  ProcessorFactory::ConstructProcessors();
  //  TriggerFactory::ConstructTriggers();
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
  
  // Return pointer to physical world volume
  return world;
}

void DetectorConstruction::ConstructSDandField()
{
  return;
}

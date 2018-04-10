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

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoManager.hh"
#include "sd/DetectorManager.hh"
#include "geo/GeoManager.hh"

using namespace COSMIC;

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(),
    fCheckOverlaps(true)
{
  DefineMaterials();
}

DetectorConstruction::~DetectorConstruction()
{}

void DetectorConstruction::DefineMaterials()
{}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Delete the old detector if we are constructing a new one
  // G4GeometryManager::GetInstance()->OpenGeometry();
  // G4PhysicalVolumeStore::GetInstance()->Clean();
  // G4LogicalVolumeStore::GetInstance()->Clean();
  // G4SolidStore::GetInstance()->Clean();

  // Load all possible sensitive detector objects first...
  // DetectorManager::Get()->ConstructAll();

  // Build entire geometry
  G4VPhysicalVolume* world = GeoManager::Get()->ConstructAll();

  // Return pointer to physical world volume
  return world;
}

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);

  // Geometry objects handle their own sensitivities...
  GeoManager::Get()->ConstructSensitive();



  return;
}
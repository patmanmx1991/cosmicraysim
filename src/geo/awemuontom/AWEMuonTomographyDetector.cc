#include "AWEMuonTomographyDetector.hh"

#include "GeoObject.hh"
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
#include "GeoUtils.hh"
#include "sd/DetectorManager.hh"
#include "simple/GeoBox.hh"
#include "simple/GeoTubs.hh"
namespace COSMIC {


void AWEMuonTomographyDetector::Construct(DBLink* table) {
  SetID(table->GetIndexName());
  SetType(table->GetS("type"));
  std::cout << "Constructing AWEMuonTomography Detector" << std::endl;
  std::string fName = table->GetIndexName();
  std::string fMotherName = table->GetS("mother");

  // Make a temporary DB from geo file
  DB* tdb = new DB("awe/muontomographydetector.geo");

  // Make the main logical volume (with mother and position/rotation overriden)
  DBLink* voltable = tdb->GetLink("GEO", "volume");
  voltable->Set("name",   fName);
  voltable->Set("mother", fMotherName);
  if (table->Has("position"))
    voltable->Set("position", table->GetVecD("position"));
  if (table->Has("rotation"))
    voltable->Set("rotation", table->GetVecD("rotation"));
  GeoObject* volume = new GeoBox( voltable );
  fSubObjects.push_back(volume);
  fLogical = volume->GetLogical();
  fPhysical = volume->GetPhysical();

  // Make the scintillator plane
  DBLink* tbl = tdb->GetLink("GEO", "scintillator_panel");
  tbl->SetIndexName(fName + "_scintillator_panel");
  GeoBox* fScintObject = new GeoBox(tbl);
  fSubObjects.push_back(fScintObject);

  // Make the drift chamber array
  DBLink* postable = tdb->GetLink("GEO", "chamber_positions");
  std::vector<std::string> fields = postable->GetFields();

  // Make array to keep track of drifts
  std::vector<GeoObject*> driftobjects;

  // Loop over all entries, look for chamber_${i}
  for (uint i = 0; i < fields.size(); i++) {
    std::string chname = fields[i];

    // skip non positions
    if (chname.find("chamber_") == std::string::npos) continue;

    // Get positoin
    std::vector<double> posrot = postable->GetVecD(chname);
    std::vector<double> position = {posrot[0], posrot[1], posrot[2]};
    std::vector<double> rotation = {posrot[3], posrot[4], posrot[5]};

    // Create the target rod by overloading table
    DBLink* chtemplate = tdb->CloneLink("GEO", "drift_chamber", fName + "_" + chname);
    chtemplate->Set("position", position);
    chtemplate->Set("rotation", rotation);
    GeoBox* obj = new GeoBox(chtemplate);
    fSubObjects.push_back(obj);
    fDriftObjects.push_back(obj);
  }

  // Register an AWEMuonTomographyProcessor to combine hits and return trigger
  if (!table->Has("processor") or table->GetB("processor")) {
    Analysis::Get()->RegisterProcessor(new AWEMuonTomographyProcessor(this));
  }

  // Remove the temporary database.
  delete tdb;
}

AWEMuonTomographyProcessor::AWEMuonTomographyProcessor(AWEMuonTomographyDetector* det) :
  VProcessor(det->GetID())
{
  fAWEDetector = det;
}

bool AWEMuonTomographyProcessor::BeginOfRunAction(const G4Run* /*run*/) {
  std::string tableindex = GetID();
  std::cout << "Registering AWEMuonTomographyProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fMuonTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
  fMuonMomXIndex = man ->CreateNtupleDColumn(tableindex + "_px");
  fMuonMomYIndex = man ->CreateNtupleDColumn(tableindex + "_py");
  fMuonMomZIndex = man ->CreateNtupleDColumn(tableindex + "_pz");
  fMuonPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
  fMuonPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
  fMuonPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");

  return true;
}

bool AWEMuonTomographyProcessor::ProcessEvent(const G4Event* /*event*/) {

  // No processors have been created for the drift chamber objects
  // We have to manually get the HitPosition from each


  // Also get the energy deposited and the hit time 



  // Need to manually get hits from all of the drift chambers sensitive volumes.

  // Then perform a likelihood fit 


  return true;
}








} // namespace COSMIC
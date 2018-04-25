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
#include "sd/LongDriftSD.hh"
namespace COSMIC {


void AWEMuonTomographyDetector::Construct(DBLink* table) {
  SetID(table->GetIndexName());
  SetType(table->GetS("type"));
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

  // DBLink* sctemplate = tdb->CloneLink("DETECTOR", "scintillator", fName + "_scintillator_panel" + "_sd");
  // ScintillatorSD* det = new ScintillatorSD(sdtemplate);

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

    // Create the target by overloading table
    DBLink* chtemplate = tdb->CloneLink("GEO", "drift_chamber", fName + "_" + chname);
    chtemplate->Set("position", position);
    chtemplate->Set("rotation", rotation);
    GeoBox* obj = new GeoBox(chtemplate);
    fSubObjects.push_back(obj);
    fDriftObjects.push_back(obj);

    // Create the sensitive detectors manually for each one
    // DBLink* sdtemplate = tdb->CloneLink("DETECTOR", "longdrift", fName + "_" + chname + "_sd");
    // LongDriftSD* det = new LongDriftSD(sdtemplate);
  }

  // // Register an AWEMuonTomographyProcessor to combine hits and return trigger
  // if (!table->Has("processor") or table->GetB("processor")) {
  //   Analysis::Get()->RegisterProcessor(new AWEMuonTomographyProcessor(this));
  // }

  // Remove the temporary database.
  delete tdb;
}

AWEMuonTomographyProcessor::AWEMuonTomographyProcessor(AWEMuonTomographyDetector* det) :
  VProcessor(det->GetID())
{
  fAWEDetector = det;
  // Get drift Geo Objects
  std::vector<GeoObject*> drifts = fAWEDetector->GetDriftObjects();

  // From each one get the sensitive and manually create a processor.
  // This is super awkward. May need to rethink.
  for (uint i = 0; i < drifts.size(); i++) {
    LongDriftSD* sd = static_cast<LongDriftSD*>(drifts[i]->GetSensitive());
    fDriftChamberProcs.push_back( new LongDriftProcessor(sd, true) );
  }


  // Create Fit Machinery
  fFitterFCN   = new TrackFitter();
  fCallFunctor = new ROOT::Math::Functor(*fFitterFCN, 6);
  fMinimizer   = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  fMinimizer->SetVariable(0, "posx", 0.0, 1.0);
  fMinimizer->SetVariable(1, "posy", 0.0, 1.0);
  fMinimizer->SetVariable(2, "posz", 0.0, 1.0);
  fMinimizer->SetVariable(3, "momx", 0.0, 1.0);
  fMinimizer->SetVariable(4, "momy", 0.0, 1.0);
  fMinimizer->SetVariable(5, "momz", 0.0, 1.0);
  fMinimizer->SetFunction(*fCallFunctor);
}

bool AWEMuonTomographyProcessor::BeginOfRunAction(const G4Run* /*run*/) {
  std::string tableindex = GetID();
  std::cout << "Registering AWEMuonTomographyProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fMuonTimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");

  fMuonPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
  fMuonPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
  fMuonPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");

  fMuonPosXIndex = man ->CreateNtupleDColumn(tableindex + "_ex");
  fMuonPosYIndex = man ->CreateNtupleDColumn(tableindex + "_ey");
  fMuonPosZIndex = man ->CreateNtupleDColumn(tableindex + "_ez");

  return true;
}

bool AWEMuonTomographyProcessor::ProcessEvent(const G4Event* event) {


  // No processors have been automatically handled for the drift chamber objects
  // We have to manually get the HitPosition from each
  std::vector<double> time;
  std::vector<G4ThreeVector> pos;
  std::vector<G4ThreeVector> err;
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {

    // Run our processing
    std::cout << "Processing drift chambre : " << i << std::endl;
    fDriftChamberProcs[i]->ProcessEvent(event);

    // Check if chamber had info
    if (!fDriftChamberProcs[i]->HasInfo()) continue;

    G4double x  = fDriftChamberProcs[i]->GetPosX();
    G4double y  = fDriftChamberProcs[i]->GetPosY();
    G4double z  = fDriftChamberProcs[i]->GetPosZ();

    // Get inverse errors for later residual calculation
    G4double ex = 1.0 / fDriftChamberProcs[i]->GetErrX();
    G4double ey = 1.0 / fDriftChamberProcs[i]->GetErrY();
    G4double ez = 1.0 / fDriftChamberProcs[i]->GetErrZ();

    G4double t  = fDriftChamberProcs[i]->GetTime();

    std::cout << " adding processed muon track " << std::endl;
    time.push_back(t);
    pos.push_back(G4ThreeVector(x, y, z));
    err.push_back(G4ThreeVector(x, y, z));
  }

  // Perform a 2D line fit in X-Y plane

  // Perform a 2D line fit in Y-Z plane 

  // Combine projection vectors to get a track

  // Require at least 3 points, with at least one alternated plane....

  

  

  // Now perform a simple track fit to return a muon vector and error
  // Reset Minuit Instance
  fMinimizer->SetVariableValue(0, 0.0);
  fMinimizer->SetVariableValue(1, 0.0);
  fMinimizer->SetVariableValue(2, 0.0);
  fMinimizer->SetVariableValue(3, 0.0);
  fMinimizer->SetVariableValue(4, 0.0);
  fMinimizer->SetVariableValue(5, 0.0);

  // Update Functor Object with hit information
  fFitterFCN->SetTimes(&time);
  fFitterFCN->SetPositions(&pos);
  fFitterFCN->SetErrors(&err);

  // Run the Fit
  fMinimizer->Minimize();

  // Extract the fit parameters
  // Get X and Err
  const double* values = fMinimizer->X();
  const double* errors = fMinimizer->Errors();

  fMuonPos    = G4ThreeVector(values[0], values[1], values[2]);
  fMuonPosErr = G4ThreeVector(errors[0], errors[1], errors[2]);
  fMuonMom    = G4ThreeVector(values[3], values[4], values[5]);
  fMuonMomErr = G4ThreeVector(errors[3], errors[4], errors[5]);
  
  return true;
}


double TrackFitter::DoEval(const double *x) const {

  G4double posx = x[0];
  G4double posy = x[1];
  G4double posz = x[2];
  G4double momx = x[3];
  G4double momy = x[4];
  G4double momz = x[5];

  G4ThreeVector pos = G4ThreeVector(x[0], x[1], x[2]);
  G4ThreeVector mom = G4ThreeVector(x[2], x[3], x[4]);
  G4ThreeVector dir = mom.unit();

  // Iterate over hits and get residual of each one
  // based on its distance away from the track and its
  // associated errors.

  // Find the closest point on the line to a given point.

  // Then use point errors to calculate a residual.

  G4double totalres = 0.0;
  for (uint i = 0; i < fHitPositions->size(); i++) {

    G4ThreeVector P = (*fHitPositions)[i];
    G4ThreeVector E = (*fHitErrors)[i];
    G4ThreeVector closest = pos + ((P-pos).dot(dir))/(dir.dot(dir)) * dir;

    // G4ThreeVector resvect = (closest - P).dot(E);
    G4double res = (closest - P).dot(E);
    totalres += res * res;
  }
  std::cout << "Total res = " << totalres << std::endl;
  return totalres;
}







} // namespace COSMIC

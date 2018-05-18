#include "AWEMuonTomographyDetectorSecond.hh"

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
#include "sd/AWEDriftSD.hh"

#include "G4VVisManager.hh"
#include "G4VisAttributes.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4Square.hh"
#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4Box.hh"
#include "db/ROOTHeaders.hh"
#include "G4Polyline.hh"
#include "Math/Functor.h"

namespace COSMIC {

// --------------------------------------------------------------------------
void AWEMuonTomographyDetectorSecond::Construct(DBTable table) {
  SetID(table.GetIndexName());
  SetType(table.GetS("type"));
  std::string fName = table.GetIndexName();
  std::string fMotherName = table.GetS("mother");

  // Load the temporary DB from geo file
  DB* tdb = DB::Get();
  tdb->CreateDataBase("awedet2");
  tdb->SelectDataBase("awedet2");
  tdb->LoadFile(DB::GetDataPath() + "/awe/muontomographydetector_second.geo");


  // Make the main logical volume (with mother and position/rotation overriden)
  DBTable voltable = DB::Get()->GetTable("GEO", "volume");
  voltable.SetTableName(fName);
  voltable.SetIndexName(fName + "_volume");
  voltable.Set("mother", fMotherName);

  if (table.Has("position"))
    voltable.Set("position", table.GetVecG4D("position"));
  if (table.Has("rotation"))
    voltable.Set("rotation", table.GetVecD("rotation"));

  GeoObject* volume = new GeoBox( voltable );
  fSubObjects.push_back(volume);
  fLogical = volume->GetLogical();
  fPhysical = volume->GetPhysical();

  // Make the scintillator plane
  std::cout << "GEO: --> Making scintillator panels" << std::endl;

  DBTable tbl = DB::Get()->GetTable("GEO", "scintillator_top");
  tbl.SetIndexName(fName + "_scintillator_top");
  tbl.Prefix("mother", fName + "_");

  fScintObjectTop = new GeoBox(tbl);
  fSubObjects.push_back(fScintObjectTop);

  tbl = DB::Get()->GetTable("GEO", "scintillator_bottom");
  tbl.SetIndexName(fName + "_scintillator_bottom");
  tbl.Prefix("mother", fName + "_");

  fScintObjectBottom = new GeoBox(tbl);
  fSubObjects.push_back(fScintObjectBottom);

  // Make the drift chamber array
  DBTable postable = DB::Get()->GetTable("GEO", "chamber_positions");
  std::vector<std::string> fields = postable.GetFields();

  std::cout << "GEO: --> Making chambers" << std::endl;
  // Loop over all entries, look for chamber_${i}
  for (uint i = 0; i < 18; i++) {
    std::string chname = "chamber_" + std::to_string(i);

    // skip non positions
    // if (chname.find("chamber_") == std::string::npos) continue;

    // Get position
    std::vector<double> posrot = postable.GetVecD(chname);
    std::vector<double> position = {posrot[0], posrot[1], posrot[2]};
    std::vector<double> rotation = {posrot[3], posrot[4], posrot[5]};

    // Create the target by overloading table
    DBTable chtemplate = DB::Get()->GetTable("GEO", "drift_chamber");
    chtemplate.SetIndexName(fName + "_" + chname);
    chtemplate.Prefix("mother", fName + "_");
    chtemplate.Set("position", position);
    chtemplate.Set("rotation", rotation);

    GeoBox* obj = new GeoBox(chtemplate);
    fSubObjects.push_back(obj);
    fDriftObjects.push_back(obj);
  }
  // Register an AWEMuonTomographyProcessorSecond to combine hits and return trigger
  if (!table.Has("processor") or table.GetB("processor")) {
    std::cout << "GEO: --> Making processor" << std::endl;
    Analysis::Get()->RegisterProcessor(new AWEMuonTomographyProcessorSecond(this));
  }

  // Remove the temporary database.
  tdb->SelectDataBase("default");
  std::cout << "GEO: --> Created AWE Detector" << std::endl;

}
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
AWEMuonTomographyProcessorSecond::AWEMuonTomographyProcessorSecond(AWEMuonTomographyDetectorSecond* det) :
  VProcessor(det->GetID()), fSave(true)
{
  fAWEDetector = det;

  // Get drift Geo Objects
  std::vector<GeoObject*> drifts = fAWEDetector->GetDriftObjects();
  G4VSensitiveDetector* sd = fAWEDetector->GetScintillatorTop()->GetSensitive();
  fScintProcTop = new SimpleScintillatorProcessor(static_cast<SimpleScintillatorSD*>(sd), true);
  sd = fAWEDetector->GetScintillatorBottom()->GetSensitive();
  fScintProcBottom = new SimpleScintillatorProcessor(static_cast<SimpleScintillatorSD*>(sd), true);

  // From each one get the sensitive and manually create a processor.
  // This is super awkward. May need to rethink.
  for (uint i = 0; i < drifts.size(); i++) {
    AWEDriftSD* sd = static_cast<AWEDriftSD*>(drifts[i]->GetSensitive());
    AWEDriftProcessor* pr = new AWEDriftProcessor(sd, true);
    std::cout << "Adding PROC : " << pr->GetID() << std::endl;
    fDriftChamberProcs.push_back( pr );
  }

}

bool AWEMuonTomographyProcessorSecond::BeginOfRunAction(const G4Run* run) {
  std::string tableindex = GetID();
  std::cout << "Registering AWEMuonTomographyProcessorSecond NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fMuonTimeIndex = man->CreateNtupleDColumn(tableindex + "_t");

  fMuonPosXIndex = man->CreateNtupleDColumn(tableindex + "_x");
  fMuonPosYIndex = man->CreateNtupleDColumn(tableindex + "_y");
  fMuonPosZIndex = man->CreateNtupleDColumn(tableindex + "_z");

  fMuonPosErrXIndex = man->CreateNtupleDColumn(tableindex + "_ex");
  fMuonPosErrYIndex = man->CreateNtupleDColumn(tableindex + "_ey");
  fMuonPosErrZIndex = man->CreateNtupleDColumn(tableindex + "_ez");

  fMuonMomXIndex = man->CreateNtupleDColumn(tableindex + "_px");
  fMuonMomYIndex = man->CreateNtupleDColumn(tableindex + "_py");
  fMuonMomZIndex = man->CreateNtupleDColumn(tableindex + "_pz");

  fMuonMomErrXIndex = man->CreateNtupleDColumn(tableindex + "_pex");
  fMuonMomErrYIndex = man->CreateNtupleDColumn(tableindex + "_pey");
  fMuonMomErrZIndex = man->CreateNtupleDColumn(tableindex + "_pez");

  fScintProcTop->BeginOfRunAction(run);
  fScintProcBottom->BeginOfRunAction(run); 
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {
    fDriftChamberProcs[i]->BeginOfRunAction(run);
  }

  return true;
}


bool AWEMuonTomographyProcessorSecond::ProcessEvent(const G4Event* event) {

  // Get the processor from the scintillator
  fScintProcTop->Reset();
  fScintProcTop->ProcessEvent(event);
  fScintProcBottom->Reset();
  fScintProcBottom->ProcessEvent(event);
  bool hasvalue = fScintProcTop->HasInfo() && fScintProcBottom->HasInfo();
  if (!hasvalue) {
    return false;
  }

  // Threshold cut
  // G4double scintenergy = fScintProc->GetEnergy();
  // std::cout << "Cint energy : " << fScintProc->GetEnergy() << std::endl;
  // if (scintenergy < 1.0 * eV) return false;

  // No processors have been automatically handled for the drift chamber objects
  // We have to manually get the HitPosition from each
  int fNHitPositions;
  std::vector<double> fTimes;
  std::vector<G4ThreeVector> fHitPositions;
  std::vector<G4ThreeVector> fHitErrors;

  std::vector<double> time;

  std::vector<double> xpos_x;
  std::vector<double> xpos_z;
  std::vector<double> xpos_xerr;
  std::vector<double> xpos_zerr;

  std::vector<double> ypos_y;
  std::vector<double> ypos_z;
  std::vector<double> ypos_yerr;
  std::vector<double> ypos_zerr;

  G4double avgx = 0.0;
  G4double avgy = 0.0;
  G4double avgz = 0.0;

  xpos_x.clear();
  xpos_z.clear();
  xpos_xerr.clear();
  xpos_zerr.clear();

  ypos_y.clear();
  ypos_z.clear();
  ypos_yerr.clear();
  ypos_zerr.clear();

  G4double averagetime = 0.0;
  for (uint i = 0; i < fDriftChamberProcs.size(); i++) {

    // Run our processing
    fDriftChamberProcs[i]->Reset();
    fDriftChamberProcs[i]->ProcessEvent(event);

    // Check if chamber had info
    if (!fDriftChamberProcs[i]->HasInfo()) continue;

    /// BAD HARD CODED LIST OF X/Y points. Fix in future.

    // X information
    if (i == 0 || i == 1 || i == 2 || i == 6 || i == 7
        || i == 8 || i == 12 || i == 13 || i == 14) {
      ypos_y.push_back(fDriftChamberProcs[i]->GetWorldPosY());
      ypos_z.push_back(fDriftChamberProcs[i]->GetWorldPosZ());
      ypos_yerr.push_back(fDriftChamberProcs[i]->GetWorldErrY());
      ypos_zerr.push_back(fDriftChamberProcs[i]->GetWorldErrZ());

      ypos_y.push_back(fDriftChamberProcs[i]->GetGhostWorldPosY());
      ypos_z.push_back(fDriftChamberProcs[i]->GetGhostWorldPosZ());
      ypos_yerr.push_back(fDriftChamberProcs[i]->GetWorldErrY());
      ypos_zerr.push_back(fDriftChamberProcs[i]->GetWorldErrZ());
    } else {
      xpos_x.push_back(fDriftChamberProcs[i]->GetWorldPosX());
      xpos_z.push_back(fDriftChamberProcs[i]->GetWorldPosZ());
      xpos_xerr.push_back(fDriftChamberProcs[i]->GetWorldErrX());
      xpos_zerr.push_back(fDriftChamberProcs[i]->GetWorldErrZ());

      xpos_x.push_back(fDriftChamberProcs[i]->GetGhostWorldPosX());
      xpos_z.push_back(fDriftChamberProcs[i]->GetGhostWorldPosZ());
      xpos_xerr.push_back(fDriftChamberProcs[i]->GetWorldErrX());
      xpos_zerr.push_back(fDriftChamberProcs[i]->GetWorldErrZ());
    }
    averagetime += fDriftChamberProcs[i]->GetTime();
  }
  averagetime /= fDriftChamberProcs.size();

  // Check at least 6 hits
  if ((xpos_x.size() != 3 ||
       ypos_y.size() != 3) and
      (xpos_x.size() != 6 ||
       ypos_y.size() != 6)) {

    // Set default values
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fMuonTimeIndex, -999.9);

    man->FillNtupleDColumn(fMuonPosXIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosYIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosZIndex, -999.9);

    man->FillNtupleDColumn(fMuonPosErrXIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosErrYIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosErrZIndex, -999.9);

    man->FillNtupleDColumn(fMuonMomXIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomYIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomZIndex, -999.9);

    man->FillNtupleDColumn(fMuonMomErrXIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomErrYIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomErrZIndex, -999.9);

    return false;
  }

  avgx /= double(xpos_x.size());
  avgy /= double(ypos_y.size());
  avgz /= double(xpos_x.size() + ypos_y.size());

// Set information
  fHasInfo = true;
  fTime    = averagetime;

// Fit the gradients
  G4double mx, mex, cx, cex, my, mey, cy, cey;
  GetMXC(mx, mex, cx, cex, xpos_z, xpos_x, xpos_xerr );
  GetMXC(my, mey, cy, cey, ypos_z, ypos_y, ypos_yerr );

// std::cout << "cx mx : " << cx << " : " << mx << std::endl;
// std::cout << "cy my : " << cy << " : " << my << std::endl;
  double values [6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double errors [6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
// values[0] = -cx-mx*avgz;
// values[1] = -cy-my*avgz;
// values[2] = avgz;
// values[3] = mx;
// values[4] = my;
// values[5] = 1.0;

  values[0] = -cx;
  values[1] = -cy;
  values[2] = 0.0;
  values[3] = mx;
  values[4] = my;
  values[5] = 1.0;

  errors[0] = 0.0;
  errors[1] = 0.0;
  errors[2] = 0.0;
  errors[3] = mex;
  errors[4] = mey;
  errors[5] = 0.0;

  fMuonPos    = G4ThreeVector(values[0], values[1], values[2]);
  fMuonPosErr = G4ThreeVector(errors[0], errors[1], errors[2]);
  fMuonMom    = G4ThreeVector(values[3], values[4], values[5]);
  fMuonMomErr = G4ThreeVector(errors[3], errors[4], errors[5]);

  fMuonMomErr *= 1.0 / fMuonMom.mag();
  fMuonMom    *= 1.0 / fMuonMom.mag();

// Draw Track if in interactive
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {
    // std::cout << "MuonPos = " << fMuonPos[0] << " " << fMuonPos[1] << " " << fMuonPos[2] << std::endl;
    // std::cout << "MuonDir = " << fMuonMom[0] << " " << fMuonMom[1] << " " << fMuonMom[2] << std::endl;
    G4Polyline polyline;
    polyline.push_back( fMuonPos + 4.0 * m * fMuonMom );
    polyline.push_back( fMuonPos );
    polyline.push_back( fMuonPos - 4.0 * m * fMuonMom );

    G4Colour colour(0., 1., 0.);
    G4VisAttributes attribs(colour);
    polyline.SetVisAttributes(attribs);

    pVVisManager->Draw(polyline);
  }

// Now fill
  if (!fSave) return false;

// If Triggered then fill
  if (fHasInfo) {

    // Fill muon vectors
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fMuonTimeIndex, fTime);

    man->FillNtupleDColumn(fMuonPosXIndex, fMuonPos[0]);
    man->FillNtupleDColumn(fMuonPosYIndex, fMuonPos[1]);
    man->FillNtupleDColumn(fMuonPosZIndex, fMuonPos[2]);

    man->FillNtupleDColumn(fMuonPosErrXIndex, fMuonPosErr[0]);
    man->FillNtupleDColumn(fMuonPosErrYIndex, fMuonPosErr[1]);
    man->FillNtupleDColumn(fMuonPosErrZIndex, fMuonPosErr[2]);

    man->FillNtupleDColumn(fMuonMomXIndex, fMuonMom[0]);
    man->FillNtupleDColumn(fMuonMomYIndex, fMuonMom[1]);
    man->FillNtupleDColumn(fMuonMomZIndex, fMuonMom[2]);

    man->FillNtupleDColumn(fMuonMomErrXIndex, fMuonMomErr[0]);
    man->FillNtupleDColumn(fMuonMomErrYIndex, fMuonMomErr[1]);
    man->FillNtupleDColumn(fMuonMomErrZIndex, fMuonMomErr[2]);

    return true;

  } else {

    // Set default values
    G4AnalysisManager* man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(fMuonTimeIndex, -999.9);

    man->FillNtupleDColumn(fMuonPosXIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosYIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosZIndex, -999.9);

    man->FillNtupleDColumn(fMuonPosErrXIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosErrYIndex, -999.9);
    man->FillNtupleDColumn(fMuonPosErrZIndex, -999.9);

    man->FillNtupleDColumn(fMuonMomXIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomYIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomZIndex, -999.9);

    man->FillNtupleDColumn(fMuonMomErrXIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomErrYIndex, -999.9);
    man->FillNtupleDColumn(fMuonMomErrZIndex, -999.9);

    return false;
  }

  return true;
}




// --------------------------------------------------------------------------
namespace GlobalFitTrackSecond {
int npoints;
G4double* pointsx;
G4double* pointsy;
G4double* errsy;
int* indices;
}


double FitTrackSecond(const double *x) {
  G4double m = x[0];
  G4double c = x[1];
  double totalres = 0.0;
  for (uint i = 0; i < GlobalFitTrackSecond::npoints; i++) {
    int iter = GlobalFitTrackSecond::indices[i];
    totalres += pow( (GlobalFitTrackSecond::pointsy[iter] - m * GlobalFitTrackSecond::pointsx[iter] + c ) / GlobalFitTrackSecond::errsy[iter], 2);
  }
  return totalres;
}


void AWEMuonTomographyProcessorSecond::GetMXC(G4double& m, G4double& me,
                                        G4double& c, G4double& ce,
                                        std::vector<G4double>& x,
                                        std::vector<G4double>& y,
                                        std::vector<G4double>& yerr) {

  GlobalFitTrackSecond::npoints = 3; //x.size();
  GlobalFitTrackSecond::pointsx = &(x[0]);
  GlobalFitTrackSecond::pointsy = &(y[0]);
  GlobalFitTrackSecond::errsy = &(yerr[0]);
  double minx = *(std::min_element(x.begin(), x.end()));
  double maxx = *(std::max_element(x.begin(), x.end()));
  double miny = *(std::min_element(y.begin(), y.end()));
  double maxy = *(std::max_element(y.begin(), y.end()));

  // Create Fit Machinery
  ROOT::Math::Minimizer* min =
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  min->SetPrintLevel(-1);
  min->SetStrategy(2);
  min->SetTolerance(0.01);
  ROOT::Math::Functor f(&FitTrackSecond, 2);
  double step[2] = {0.01, 0.01};
  double variable[2] = {(maxy - miny) / (maxx - minx), (maxy + miny) / 2.0 };
  min->SetFunction(f);

  double maxgrad = 20 * (maxy - miny) / (maxx - minx);
  double maxc = 2 * (fabs(miny) + fabs(maxy));

  std::vector<std::vector<int>> pairwise;
  if (x.size() == 6) {
    pairwise.push_back(std::vector<int> {0, 2, 4}); // 000
    pairwise.push_back(std::vector<int> {0, 2, 5}); // 001
    pairwise.push_back(std::vector<int> {0, 3, 4}); // 010
    pairwise.push_back(std::vector<int> {1, 2, 4}); // 100
    pairwise.push_back(std::vector<int> {0, 3, 5}); // 011
    pairwise.push_back(std::vector<int> {1, 2, 5}); // 101
    pairwise.push_back(std::vector<int> {1, 3, 4}); // 110
    pairwise.push_back(std::vector<int> {1, 3, 5}); // 111

  } else {
    pairwise.push_back(std::vector<int> {0, 1, 2});
  }

  double bestfit = -1.0;
  for (int i = 0; i < pairwise.size(); i++) {

    // std::cout << "INDICES " << pairwise[i][0] << " " << x.size() << std::endl;
    GlobalFitTrackSecond::indices = &(pairwise[i][0]);

    min->SetVariable(0, "m", variable[0], step[0]);
    min->SetVariable(1, "c", variable[1], step[1]);
    min->Minimize();


    const double *xs = min->X();
    const double *es = min->Errors();
    double chi2 = FitTrackSecond(xs);
    if (bestfit < 0 or chi2 < bestfit) {
      m = xs[0];
      c = xs[1];
      me = es[0];
      ce = es[1];
      bestfit = chi2;
    }
  }


  delete min;
  return;
}

// --------------------------------------------------------------------------
} // namespace COSMIC
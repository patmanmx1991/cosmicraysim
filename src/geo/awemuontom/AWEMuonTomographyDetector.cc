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

#include "Minuit2/Minuit2Minimizer.h"
#include "Math/Functor.h"

namespace COSMIC {

// --------------------------------------------------------------------------
void AWEMuonTomographyDetector::Construct(DBTable table) {
  SetID(table.GetIndexName());
  SetType(table.GetS("type"));
  std::string fName = table.GetIndexName();
  std::string fMotherName = table.GetS("mother");

  // Load the temporary DB from geo file
  DBNEW* tdb = DBNEW::Get();
  tdb->CreateDataBase("vsc24");
  tdb->SelectDataBase("vsc24");
  tdb->LoadFile(DBNEW::GetDataPath() + "/awe/muontomographydetector.geo");


  // Make the main logical volume (with mother and position/rotation overriden)
  DBTable voltable = DBNEW::Get()->GetTable("GEO", "volume");
  voltable.Set("name",   fName);
  voltable.Set("mother", fMotherName);
  if (table.Has("position"))
    voltable.Set("position", table.GetVecD("position"));
  if (table.Has("rotation"))
    voltable.Set("rotation", table.GetVecD("rotation"));

  GeoObject* volume = new GeoBox( voltable );
  fSubObjects.push_back(volume);
  fLogical = volume->GetLogical();
  fPhysical = volume->GetPhysical();

  // Make the scintillator plane
  DBTable tbl = DBNEW::Get()->GetTable("GEO", "scintillator_panel");
  tbl.SetIndexName(fName + "_scintillator_panel");
  fScintObject = new GeoBox(tbl);
  fSubObjects.push_back(fScintObject);

  // Make the drift chamber array
  DBTable postable = DBNEW::Get()->GetTable("GEO", "chamber_positions");
  std::vector<std::string> fields = postable.GetFields();

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
    DBTable chtemplate = DBNEW::Get()->GetTable("GEO", "drift_chamber");
    chtemplate.SetIndexName(fName + "_" + chname);
    chtemplate.Set("position", position);
    chtemplate.Set("rotation", rotation);

    GeoBox* obj = new GeoBox(chtemplate);
    fSubObjects.push_back(obj);
    fDriftObjects.push_back(obj);
  }

  // Register an AWEMuonTomographyProcessor to combine hits and return trigger
  if (!table.Has("processor") or table.GetB("processor")) {
    Analysis::Get()->RegisterProcessor(new AWEMuonTomographyProcessor(this));
  }

  // Remove the temporary database.
  tdb->SelectDataBase("default");
  std::cout << "GEO: --> Created DRY Storage Cask" << std::endl;
}
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
AWEMuonTomographyProcessor::AWEMuonTomographyProcessor(AWEMuonTomographyDetector* det) :
  VProcessor(det->GetID())
{
  fAWEDetector = det;
  // Get drift Geo Objects
  std::vector<GeoObject*> drifts = fAWEDetector->GetDriftObjects();
  G4VSensitiveDetector* sd = fAWEDetector->GetScintillatorObject()->GetSensitive();
  fScintProc = new SimpleScintillatorProcessor(static_cast<SimpleScintillatorSD*>(sd));

  // From each one get the sensitive and manually create a processor.
  // This is super awkward. May need to rethink.
  for (uint i = 0; i < drifts.size(); i++) {
    LongDriftSD* sd = static_cast<LongDriftSD*>(drifts[i]->GetSensitive());
    fDriftChamberProcs.push_back( new LongDriftProcessor(sd, false) );
  }

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

bool AWEMuonTomographyProcessor::ProcessEvent(const G4Event* event) {

  // Get the processor from the scintillator
  fScintProc->ProcessEvent(event);
  if (!fScintProc->HasInfo()) return false;

  // Threshold cut
  G4double scintenergy = fScintProc->GetEnergy();
  if (scintenergy < 1.0 * eV) return false;

  // No processors have been automatically handled for the drift chamber objects
  // We have to manually get the HitPosition from each


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
    fDriftChamberProcs[i]->ProcessEvent(event);

    // Check if chamber had info
    if (!fDriftChamberProcs[i]->HasInfo()) continue;

    /// BAD HARD CODED LIST OF X/Y points. Fix in future.

    // X information
    if (i == 0 || i == 1 || i == 2 || i == 9 || i == 10
        || i == 11 || i == 12 || i == 13 || i == 14) {
      // std::cout << "XHIT : " << i << fDriftChamberProcs[i]->GetID() << std::endl;
      xpos_x.push_back(fDriftChamberProcs[i]->GetPosX());
      xpos_z.push_back(fDriftChamberProcs[i]->GetPosZ());
      xpos_xerr.push_back(fDriftChamberProcs[i]->GetErrX());
      xpos_zerr.push_back(fDriftChamberProcs[i]->GetErrZ());
      avgx += fDriftChamberProcs[i]->GetPosX();
      avgz += fDriftChamberProcs[i]->GetErrZ();
    } else {
      // std::cout << "YHIT : " << i << " " << fDriftChamberProcs[i]->GetID() << std::endl;
      ypos_y.push_back(fDriftChamberProcs[i]->GetPosY());
      ypos_z.push_back(fDriftChamberProcs[i]->GetPosZ());
      ypos_yerr.push_back(fDriftChamberProcs[i]->GetErrY());
      ypos_zerr.push_back(fDriftChamberProcs[i]->GetErrZ());
      avgy += fDriftChamberProcs[i]->GetPosX();
      avgz += fDriftChamberProcs[i]->GetErrZ();
    }
    averagetime += fDriftChamberProcs[i]->GetTime();
  }
  averagetime /= fDriftChamberProcs.size();

  // Check at least 6 hits
  // std::cout << "NHITS = " << xpos_x.size() << " " <<  ypos_y.size() << std::endl;
  if (xpos_x.size() < 3) return false;
  if (ypos_y.size() < 3) return false;

  avgx /= double(xpos_x.size());
  avgy /= double(ypos_y.size());
  avgz /= double(xpos_x.size() + ypos_y.size());

  // Set information
  fHasInfo = true;
  fTime    = averagetime;

  G4double minx = *(std::min_element(xpos_x.begin(), xpos_x.end()));
  G4double miny = *(std::min_element(ypos_y.begin(), ypos_y.end()));
  G4double minz = *(std::min_element(ypos_z.begin(), ypos_z.end()));
  G4double maxx = *(std::max_element(xpos_x.begin(), xpos_x.end()));
  G4double maxy = *(std::max_element(ypos_y.begin(), ypos_y.end()));
  G4double maxz = *(std::max_element(ypos_z.begin(), ypos_z.end()));


  // TFitterMinuit * minuit = new TFitterMinuit(6);
  // fFitterFCN   = new TrackFitter();
  // minuit->SetMinuitFCN(fFitterFCN);
  // double fitvalues[6] = {avgx, avgy, avgz, 1.0, 1.0, 1.0};

  // minuit->SetParameter(0, "posx", fitvalues[0], 0.1, 0,0);
  // minuit->SetParameter(1, "posy", fitvalues[1], 0.1, 0,0);
  // minuit->SetParameter(2, "posz", fitvalues[2], 0.1, 0,0);
  // minuit->SetParameter(3, "momx", fitvalues[3], 0.1, 0,0);
  // minuit->SetParameter(4, "momy", fitvalues[4], 0.1, 0,0);
  // minuit->SetParameter(5, "momz", fitvalues[5], 0.1, 0,0);
  // minuit->SetPrintLevel(3);
  // minuit->CreateMinimizer();
  // minuit->Minimize();


  // double values[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  // double errors[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  // for (uint i = 0; i < 6; i++){
  //   values[i] = minuit->GetParameter(i);
  //   values[i] = minuit->GetParError(i);
  // }


  G4double mx, cx, my, cy;
  GetMXC(mx, cx, xpos_z, xpos_x, xpos_xerr );
  GetMXC(my, cy, ypos_z, ypos_y, ypos_yerr );




  // // fMinimizer->FixVariable(0);
  // fMinimizer->FixVariable(1);
  // fMinimizer->FixVariable(2);
  // fMinimizer->FixVariable(3);
  // fMinimizer->FixVariable(4);
  // fMinimizer->FixVariable(5);


  // Update Functor Object with hit information
  // fFitterFCN->SetNHits(time.size());


  double values [6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  double errors [6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  values[0] = 0.0;
  values[1] = 0.0;
  values[2] = cx;
  values[3] = 1.0;
  values[4] = 0.0;
  values[5] = mx;

  fMuonPos    = G4ThreeVector(values[0], values[1], values[2]);
  fMuonPosErr = G4ThreeVector(errors[0], errors[1], errors[2]);
  fMuonMom    = G4ThreeVector(values[3], values[4], values[5]);
  fMuonMomErr = G4ThreeVector(0.0, 0.0, 0.0);

  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {

    std::cout << "MuonPos = " << fMuonPos[0] << " " << fMuonPos[1] << " " << fMuonPos[2] << std::endl;
    std::cout << "MuonDir = " << fMuonMom[0] << " " << fMuonMom[1] << " " << fMuonMom[2] << std::endl;
    G4Polyline polyline;
    polyline.push_back( fMuonPos + 4.0 * m * fMuonMom );
    polyline.push_back( fMuonPos - 4.0 * m * fMuonMom );

    G4Colour colour(1., 1., 0.);
    G4VisAttributes attribs(colour);
    polyline.SetVisAttributes(attribs);

    pVVisManager->Draw(polyline);
  }


  return true;
}


// --------------------------------------------------------------------------
std::vector<double> pointsx;
std::vector<double> pointsy;
std::vector<double> errsy;

double FitTrack(const double *x) {
  G4double m = x[0];
  G4double c = x[1];
  double totalres = 0.0;
  for (uint i = 0; i < pointsy.size(); i++) {
    std::cout << pointsy[i] << std::endl;
    totalres += pow( (pointsy[i] - m * pointsx[i] + c ), 2);
  }
  return totalres;
}


void AWEMuonTomographyProcessor::GetMXC(G4double& m, G4double& c, std::vector<G4double>& x, std::vector<G4double>& y, std::vector<G4double>& yerr) {

  pointsx = x;
  pointsy = y;
  errsy = yerr;
  double minx = *(std::min_element(pointsx.begin(),pointsx.end()));
  double maxx = *(std::max_element(pointsx.begin(),pointsx.end()));
  double miny = *(std::min_element(pointsy.begin(),pointsy.end()));
  double maxy = *(std::max_element(pointsy.begin(),pointsy.end()));

  // Create Fit Machinery
  ROOT::Math::Minimizer* min = 
            ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  // ROOT::Minuit2::Minuit2Minimizer min (ROOT::Minuit2::kMigrad );
  ROOT::Math::Functor f(&FitTrack, 2);
  double step[2] = {0.01,0.01};
  double variable[2] = {(maxy-miny)/(maxx-minx), pointsy[0]};
  min->SetFunction(f);

  min->SetVariable(0, "m", variable[0], step[0]);
  min->SetVariable(1, "c", variable[1], step[1]);

  min->Minimize();

  const double *xs = min->X();
  m = xs[0];
  c = xs[1];



  // // Create Fit Machinery
  // TrackFitter* newtracker = new TrackFitter();
  // // ROOT::Math::Functor* fCallFunctor = new ROOT::Math::Functor(newtracker, 2);
  // ROOT::Math::Minimizer* fMinimizer   = ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");
  // fMinimizer->SetVariable(0, "m", 1.0, 100.0);
  // fMinimizer->SetVariable(1, "c", 1.0, 100.0);
  // // fMinimizer->SetFunction(*fCallFunctor);

  // // Now perform a simple track fit to return a muon vector and error
  // // Reset Minuit Instance

  // // // Run the Fit
  // fMinimizer->Minimize();

  // // // Extract the fit parameters
  // // // Get X and Err
  // const double* values = fMinimizer->X();
  // const double* errors = fMinimizer->Errors();

  // m = values[0];
  // c = values[1];

  // delete fMinimizer;
  // delete fCallFunctor;

  return;
}

double TrackFitter::DoEvalS(const double *x) const {
  return 0.0;
}
//   G4double m = x[0];
//   G4double c = x[1];
//   double totalres = 0.0;
//   for (uint i = 0; i < pointsy.size(); i++){
//     totalres += pow( (pointsy[i] - m*pointsx[i]+c ) / errsy[i],2);
//   }
//   return totalres;
// }


// G4double posx = x[0];
// G4double posy = x[1];
// G4double posz = x[2];
// G4double momx = x[3];
// G4double momy = x[4];
// G4double momz = x[5];

// std::cout << "pos : " << posx << " " << posy << " " << posz << std::endl;
// std::cout << "mom : " << momx << " " << momy << " " << momz << std::endl;

// G4double totalres = 0.0;
// for (uint i = 0; i < xpos_x.size(); i++){
//   G4double t = (xpos_z[i] - posz) / momz;
//   G4double pointinxplane = posx + t*momx;
//   totalres += pow( (xpos_x[i] - pointinxplane) / xpos_xerr[i], 2);
// }
//   for (uint i = 0; i < ypos_y.size(); i++){
//   G4double t = (ypos_z[i] - posz) / momz;
//   G4double pointinyplane = posy + t*momy;
//   totalres += pow( (ypos_y[i] - pointinyplane) / ypos_yerr[i], 2);
// }
// std::cout << "Totalres : " << totalres << std::endl;
// return totalres;












// int ycount = 0;
// int xcount = 0;
// for (int i = 0; i < fHitPositions.size(); i++) {

//   // Get X residual
//   std::cout << "X ERROR : " << fHitErrors[i][0] << std::endl;
//   if (fHitErrors[i][0] > 0.0) {
//     xcount++;
//     G4double pointinx = (fHitPositions[i][1] - posy) / momy;
//     G4double resinx = (pointinx - fHitPositions[i][0]) / fHitErrors[i][0];
//     totalres += resinx * resinx;
//   }

//   // Get X residual
//   std::cout << "Y ERROR : " << fHitErrors[i][1] << std::endl;
//   if (fHitErrors[i][1] > 0.0) {
//     ycount++;
//     G4double pointiny = (fHitPositions[i][0] - posx) / momx;
//     G4double resiny = (pointiny - fHitPositions[i][1]) / fHitErrors[i][1];
//     totalres += resiny * resiny;
//   }

// }
// std::cout << " Total res : " << totalres << " : " << xcount << " : " << ycount << std::endl;
// return totalres;








//   G4ThreeVector Q = pos + 100.0 * dir;
//   G4ThreeVector R = pos - 100.0 * dir;

//   G4double totalres = 0.0;
// // fNHitPositions = fHitPositions.size();
// // if (!fNHitPositions){
// //   std::cout << "CANT FIND HTIS" << std::endl;
// //   throw;
// // }
//   for (uint i = 0; i < fHitPositions.size(); i++) {
//     G4ThreeVector P = G4ThreeVector((fHitPositions[i]));
//     G4ThreeVector E = G4ThreeVector((fHitErrors[i]));

//     G4double t = (R - Q).dot(Q - P) / (R - Q).dot(R - Q);
//     G4ThreeVector C = Q + t * (R - Q);

//     G4double res = (C - P).dot(E);
//     totalres += res * res;

//   }
//   std::cout << "Total res : " << totalres << std::endl;
//   return totalres;


// // // Iterate over hits and get residual of each one
// // // based on its distance away from the track and its
// // // associated errors.

// // // Find the closest point on the line to a given point.

// // // Then use point errors to calculate a residual.

// // G4double totalres = 0.0;
// // for (uint i = 0; i < fHitPositions->size(); i++) {

// //   G4ThreeVector P = (*fHitPositions)[i];
// //   G4ThreeVector E = (*fHitErrors)[i];
// //   G4ThreeVector closest = pos + ((P-pos).dot(dir))/(dir.dot(dir)) * dir;

// //   G4double res = (closest - P).dot((closest - P));
// //   totalres += res * res;
// // }
// // totalres = pos.dot(mom);
// // std::cout << "Total res = " << pos.dot(mom) << std::endl;
// // return totalres;

// --------------------------------------------------------------------------
} // namespace COSMIC

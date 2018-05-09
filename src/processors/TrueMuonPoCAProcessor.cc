#include "TrueMuonPoCAProcessor.hh"

#include "analysis/Analysis.hh"
#include "db/DBTable.hh"



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


namespace COSMIC {

//------------------------------------------------------------------
TrueMuonPoCAProcessor::TrueMuonPoCAProcessor(DBTable tbl) :
  VProcessor(tbl.GetIndexName()), fAngleThreshold(0.0)
{
  fTrackerA = static_cast<TrueMuonTracker*>(Analysis::Get()->GetDetector(tbl.GetS("trackerA")));
  fTrackerB = static_cast<TrueMuonTracker*>(Analysis::Get()->GetDetector(tbl.GetS("trackerB")));
  if (tbl.Has("angle_threshold")) fAngleThreshold = tbl.GetG4D("angle_threshold");
}

bool TrueMuonPoCAProcessor::BeginOfRunAction(const G4Run* /*run*/) {

  std::string tableindex = GetID();
  std::cout << "DET: Registering TrueMuonPoCAProcessor NTuples " << tableindex << std::endl;

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  // Fill index energy
  fPoCATimeIndex = man ->CreateNtupleDColumn(tableindex + "_t");
  fPoCAPosXIndex = man ->CreateNtupleDColumn(tableindex + "_x");
  fPoCAPosYIndex = man ->CreateNtupleDColumn(tableindex + "_y");
  fPoCAPosZIndex = man ->CreateNtupleDColumn(tableindex + "_z");
  fPoCADistIndex = man ->CreateNtupleDColumn(tableindex + "_ds");
  fPoCAAnglIndex = man ->CreateNtupleDColumn(tableindex + "_th");

  return true;
}

bool TrueMuonPoCAProcessor::ProcessEvent(const G4Event* /*event*/) {

  // Set Ntuple to defaults
  G4AnalysisManager* man = G4AnalysisManager::Instance();
  man->FillNtupleDColumn(fPoCATimeIndex, -999.);
  man->FillNtupleDColumn(fPoCAPosXIndex, -999.);
  man->FillNtupleDColumn(fPoCAPosYIndex, -999.);
  man->FillNtupleDColumn(fPoCAPosZIndex, -999.);
  man->FillNtupleDColumn(fPoCADistIndex, -999 );
  man->FillNtupleDColumn(fPoCAAnglIndex, -999 );

  // Get PoCA Variables from Tracker
  fHasInfo = (fTrackerA->GetMuonPDG() != -999 and
              fTrackerB->GetMuonPDG() != -999);

  if (fHasInfo) {

    // Set Standard Variables
    fTime   = (fTrackerA->GetMuonTime() + fTrackerB->GetMuonTime() ) / 2.0;
    fEnergy = -999.;

    // Get the A and B positions and momentums
    G4ThreeVector posA = fTrackerA->GetMuonPos();
    G4ThreeVector momA = fTrackerA->GetMuonMom();
    G4ThreeVector posB = fTrackerB->GetMuonPos();
    G4ThreeVector momB = fTrackerB->GetMuonMom();

    // Get the scatter angle
    fAngle = acos( (momB.dot(momA))/(momB.mag()*momA.mag()) ) * 180. / (2.*3.142);

    if (fAngle < fAngleThreshold){
      fHasInfo = false;
      return false;
    }

    // Get the PoCA
    G4ThreeVector w0 = posA - posB;
    double a = momA.dot(momA);
    double b = momA.dot(momB);
    double c = momB.dot(momB);
    double d = momA.dot(w0);
    double e = momB.dot(w0);

    double sc = ((b*e) - (c*d))/((a*c)-(b*b));
    double tc = ((a*e) - (b*d))/((a*c)-(b*b));

    fPoCA = 0.5 * (posA + sc*momA + posB + tc*momB);
    fDistance = (posA + sc*momB - posB - tc*momB).mag();

    // Fill muon vectors
    man->FillNtupleDColumn(fPoCATimeIndex, fTime   / ms);
    man->FillNtupleDColumn(fPoCAPosXIndex, fPoCA[0] / m);
    man->FillNtupleDColumn(fPoCAPosYIndex, fPoCA[1] / m);
    man->FillNtupleDColumn(fPoCAPosZIndex, fPoCA[2] / m);
    man->FillNtupleDColumn(fPoCADistIndex, fDistance);
    man->FillNtupleDColumn(fPoCAAnglIndex, fAngle);

    DrawEvent();

    return true;
  } else {
    return false;
  }
}


void TrueMuonPoCAProcessor::DrawEvent(){
  // Draw Track if in interactive
  G4VVisManager* pVVisManager = G4VVisManager::GetConcreteInstance();
  if (pVVisManager)
  {

    G4Polyline polyline;
    polyline.push_back( fTrackerA->GetMuonPos() );
    polyline.push_back( fPoCA );
    polyline.push_back( fTrackerB->GetMuonPos() );
    G4Colour colour(0., 0., 1.);
    G4VisAttributes attribs(colour);
    polyline.SetVisAttributes(attribs);
    pVVisManager->Draw(polyline);

    G4Square square(fPoCA);
    square.SetScreenSize(6);
    square.SetFillStyle(G4Square::filled);
    G4Colour coloursquare(1., 0., 0.);
    G4VisAttributes attribssquare(coloursquare);
    square.SetVisAttributes(attribssquare);
    pVVisManager->Draw(square);
  }
}



} // - namespace COSMIC

#include "TrueMuonPoCAProcessor.hh"

#include "analysis/Analysis.hh"
#include "db/DBLink.hh"

namespace COSMIC {

//------------------------------------------------------------------
TrueMuonPoCAProcessor::TrueMuonPoCAProcessor(DBLink* tbl) :
  VProcessor(tbl->GetIndexName())
{
  fTrackerA = static_cast<TrueMuonTracker*>(Analysis::Get()->GetDetector(tbl->GetS("trackerA")));
  fTrackerB = static_cast<TrueMuonTracker*>(Analysis::Get()->GetDetector(tbl->GetS("trackerB")));
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
    double angle = acos( (momB.dot(momA))/(momB.mag()*momA.mag()) ) * 180. / (2.*3.142);

    // Get the PoCA
    G4ThreeVector w0 = posA - posB;
    double a = momA.dot(momA);
    double b = momA.dot(momB);
    double c = momB.dot(momB);
    double d = momA.dot(w0);
    double e = momB.dot(w0);

    double sc = ((b*e) - (c*d))/((a*c)-(b*b));
    double tc = ((a*e) - (b*d))/((a*c)-(b*b));

    G4ThreeVector poca = 0.5 * (posA + sc*momA + posB + tc*momB);
    double distance = (posA + sc*momB - posB - tc*momB).mag();

    // Fill muon vectors
    man->FillNtupleDColumn(fPoCATimeIndex, fTime   / ms);
    man->FillNtupleDColumn(fPoCAPosXIndex, poca[0] / m);
    man->FillNtupleDColumn(fPoCAPosYIndex, poca[1] / m);
    man->FillNtupleDColumn(fPoCAPosZIndex, poca[2] / m);
    man->FillNtupleDColumn(fPoCADistIndex, distance);
    man->FillNtupleDColumn(fPoCAAnglIndex, angle);

    return true;
  } else {
    return false;
  }
}

} // - namespace COSMIC
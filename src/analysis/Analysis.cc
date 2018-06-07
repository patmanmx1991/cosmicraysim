#include "Analysis.hh"

#include "g4root.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"

#include "db/DB.hh"
#include "VProcessor.hh"
#include "VTrigger.hh"
#include "VFluxProcessor.hh"
#include "VDetector.hh"

namespace COSMIC {

Analysis::Analysis() :
  fFluxProcessor(0),
  fSavedEvents(0),
  fSavedEventsLimit(-1),
  fGeneratedEvents(0),
  fGeneratedEventsLimit(-1),
  fExposureTimeLimit(-1.0),
  fNTuplesSetup(0),
  fChunkSize(10000),
  fRunMode(kEventMode),
  fLastCount(0)
{
}

Analysis::~Analysis()
{
}

Analysis *Analysis::fPrimary(0);


void Analysis::BeginOfRunAction(const G4Run* run) {

  // Setup Ntuples
  if (!fNTuplesSetup) {
    std::cout << "ANL: Setting up G4Manager" << std::endl;
    // Initialise G4 ROOT Manager
    fG4Manager = G4AnalysisManager::Instance();
    fG4Manager->SetVerboseLevel(1);
    fG4Manager->SetFirstNtupleId(0);
    fG4Manager->SetFirstHistoId(0);
    fG4Manager->CreateNtuple("detectorevents", "Dynamic Detector Saved Information");


    // Run event processors begin run action
    std::vector<VProcessor*>::iterator iter;
    for (iter = fProcessors.begin(); iter != fProcessors.end(); iter++) {
      (*iter)->BeginOfRunAction(run);
    }

    // Run Flux Processor begin run action
    if (fFluxProcessor) fFluxProcessor->BeginOfRunAction(run);

    // Finish Ntuple and open output
    fG4Manager->FinishNtuple();
  }

  // Make new output file
  std::string outputname = "";
  outputname += fOutputTag + ".";
  outputname += std::to_string(fRunID) + ".";
  outputname += std::to_string(fSubRunID) + ".root";

  // Open new output
  fG4Manager->OpenFile(outputname);
  fNTuplesSetup = true;
}

void Analysis::EndOfRunAction(const G4Run* /*run*/) {
  // Skip Dummy Runs
  std::cout << "Writing" << std::endl;
  fG4Manager->Write();
  fG4Manager->CloseFile();
}

void Analysis::BeginOfEventAction() {
  ResetState();
}

void Analysis::ProcessEvent(const G4Event* event) {

  // Run Flux Processor
  if (fFluxProcessor) fFluxProcessor->ProcessEvent(event);

  // Run Event processors
  std::vector<VProcessor*>::iterator piter;
  for (piter = fProcessors.begin(); piter != fProcessors.end(); piter++) {
    (*piter)->ProcessEvent(event);
  }

  // Run Trigger Processors
  std::vector<VTrigger*>::iterator titer;
  for (titer = fTriggers.begin(); titer != fTriggers.end(); titer++) {
    (*titer)->ProcessTrigger(event);
  }

  // Add one to total generated
  fGeneratedEvents++;

  // If we have at least one trigger record the event
  if (IsTriggered()) RecordEvent();

}

void Analysis::RecordEvent() {
  fG4Manager->AddNtupleRow();
  fSavedEvents++;
}

void Analysis::ResetState() {
  ResetTriggers();
  ResetProcessors();
  ResetDetectors();
}

void Analysis::RegisterTrigger(VTrigger* t) {
  fTriggers.push_back(t);
}

bool Analysis::IsTriggered() {

  // If no valid triggers loaded, save everything
  if (fTriggers.size() < 1) return true;

  // For now just find at least one true trigger
  std::vector<VTrigger*>::iterator iter;
  for (iter = fTriggers.begin(); iter != fTriggers.end(); iter++) {
    if ((*iter)->GetTrigger()) {
      return true;
    }
  }

  return false;
}

void Analysis::ResetTriggers() {
  std::vector<VTrigger*>::iterator iter;
  for (iter = fTriggers.begin(); iter != fTriggers.end(); iter++) {
    (*iter)->Reset();
  }
}

void Analysis::DestroyTriggers() {
  for (uint i = 0; i < fTriggers.size(); i++) {
    delete (fTriggers[i]);
  }
}

VTrigger* Analysis::GetTrigger(std::string id, bool silentfail) {
  std::vector<VTrigger*>::iterator iter;
  for (iter = fTriggers.begin(); iter != fTriggers.end(); iter++) {
    std::string trigid = (*iter)->GetID();
    if (id.compare(trigid) == 0) return (*iter);
  }
  if (!silentfail) {
    std::cout << "Cannot find Trigger : " << id << std::endl;
    throw;
  }
  return 0;
}

void Analysis::RegisterProcessor(VProcessor* p) {
  fProcessors.push_back(p);
}

void Analysis::ResetProcessors() {
  std::vector<VProcessor*>::iterator iter;
  for (iter = fProcessors.begin(); iter != fProcessors.end(); iter++) {
    (*iter)->Reset();
  }
}

void Analysis::DestroyProcessors() {
  for (uint i = 0; i < fProcessors.size(); i++) {
    delete fProcessors[i];
  }
}

VProcessor* Analysis::GetProcessor(std::string id, bool silentfail) {
  std::vector<VProcessor*>::iterator iter;
  for (iter = fProcessors.begin(); iter != fProcessors.end(); iter++) {
    std::string procid = (*iter)->GetID();
    if (id.compare(procid) == 0) return (*iter);
  }
  if (!silentfail) {
    std::cout << "Cannot find Processor : " << id << std::endl;
    throw;
  }
  return 0;
}

void Analysis::RegisterDetector(VDetector* p) {
  fDetectors.push_back(p);
  G4SDManager::GetSDMpointer()->AddNewDetector(static_cast<G4VSensitiveDetector*>(p));
}

void Analysis::ResetDetectors() {
  std::vector<VDetector*>::iterator iter;
  for (iter = fDetectors.begin(); iter != fDetectors.end(); iter++) {
    (*iter)->ResetState();
  }
}

void Analysis::DestroyDetectors() {
  for (uint i = 0; i < fDetectors.size(); i++) {
    delete fDetectors[i];
  }
}

VDetector* Analysis::GetDetector(std::string id, bool silentfail) {
  std::vector<VDetector*>::iterator iter;
  for (iter = fDetectors.begin(); iter != fDetectors.end(); iter++) {
    std::string detid = (*iter)->GetID();
    if (id.compare(detid) == 0) return (*iter);
  }
  if (!silentfail) {
    std::cout << "Cannot find Detector : " << id << std::endl;
    throw;
  }
  return 0;
}

void Analysis::SetFluxProcessor(VFluxProcessor* p) {
  fFluxProcessor = p;
}

void Analysis::ResetCounters() {
  fSavedEvents = 0;
  fGeneratedEvents = 0;
  fFluxProcessor->ResetExposureTime();
}

double Analysis::GetEventRate() {
  return fFluxProcessor->GetEventRate();
}

void Analysis::CheckAbortState() {
  if (fRunMode == kTimeExposureMode) {
    if (GetExposureTime() > fRequiredExposure) {
      G4RunManager::GetRunManager()->AbortRun(true);
    }
  }
  if (fRunMode == kTriggerMode) {
    if (fSavedEvents > fRequiredTriggers) {
      G4RunManager::GetRunManager()->AbortRun(true);
    }
  }
  return;
}

void Analysis::PrintProgress(int curcount, int totalcount) {

  // Only print every 1000 events regardless
  if (curcount % 1000 != 0 or curcount == 0) return;

  // Get time taken so far
  long int curtime = time(0);
  long int prctime = curtime - fStartTime;

  // Print N Events
  if (fRunMode == kEventMode) {

    double remtime = double(totalcount*prctime/double(curcount));

    if (curcount - fLastCount > totalcount/20.0) {
      std::cout << "RUN: --> Processing Event : " << curcount << " after " << prctime / 60 << " min. "
                << "Approx. " << remtime / 60 << " min remaining." << std::endl;
      fLastCount = curcount;
    }

  } else if (fRunMode == kTimeExposureMode){

    double curexposure = GetExposureTime();
    double remtime = double((fRequiredExposure-curexposure)*prctime/curexposure);

    if (curexposure - fLastCount > (fRequiredExposure/20.0)) {
      std::cout << "RUN: --> Processing Event : " << curcount << " after " << prctime/60.0  << " minutes. "
                << "Approx. " << remtime/60.0  << " min remaining. Exposure : " << int(curexposure) << "/" << fRequiredExposure << std::endl;
      fLastCount = curexposure;
    }
  } else if (fRunMode == kTriggerMode){

    double remtime = double((fRequiredTriggers-fSavedEvents)*prctime/fSavedEvents);
    if (fSavedEvents - fLastCount > (fRequiredTriggers/20.0)){
      std::cout << "RUN: --> Processing Event : " << curcount << " after " << prctime/60.0  << " minutes. "
                << "Approx. " << remtime/60.0  << " min remaining. Triggers : " << fSavedEvents << "/" << fRequiredTriggers << std::endl;
      fLastCount = fSavedEvents;
    }


  }
}

} // - namespace COSMIC




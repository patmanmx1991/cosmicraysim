#include "CoincidenceTrigger.hh"

#include "G4Event.hh"

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "analysis/VTrigger.hh"
#include "analysis/VProcessor.hh"
#include "analysis/Analysis.hh"

namespace COSMIC {

CoincidenceTrigger::CoincidenceTrigger(DBTable tbl)
  : VTrigger(tbl.GetIndexName())
{
  std::cout << "TRG: Loading Coincidence Trigger : " << tbl.GetIndexName() << std::endl;

  // Get list of detector objects
  if (tbl.Has("processors")){
    SetupProcessors(tbl.GetVecS("processors"));
  }

  // Get thresholds
  fEnergyThreshold = -1.0;
  if (tbl.Has("energy_threshold")) {
    fEnergyThreshold = tbl.GetD("energy_threshold") * MeV;
    std::cout << "TRG: Setting Energy Threshold : " << fEnergyThreshold << std::endl;
  }

  if (tbl.Has("require_n")){
    fRequireN = tbl.GetI("require_n");
    std::cout << "TRG: Setting Coincident Detectors : " << fRequireN << std::endl;
  }


}

CoincidenceTrigger::CoincidenceTrigger(std::string name,
                             std::vector<std::string> processors,
                             G4int N, G4double energy_thresh) : VTrigger(name)
{

  SetupProcessors(processors);

  fRequireN = N;
  fEnergyThreshold = energy_thresh;

}

void CoincidenceTrigger::SetupProcessors(std::vector<std::string> det) {
  for (uint i = 0; i < det.size(); i++) {
    VProcessor* detobj = Analysis::Get()->GetProcessor(det[i]);
    fProcessors.push_back(detobj);
    std::cout << " --> Processor Input " << i << " : " << det[i] << std::endl;
  }
}

bool CoincidenceTrigger::ProcessTrigger(const G4Event* /*event*/) {

  // Trigger requires all detectors to be triggered within a certain time window
  bool complete_trig = false;
  int triggers = 0;
  for (uint i = 0; i < fProcessors.size(); i++){
    G4double ener = fProcessors[i]->GetEnergy();
    if (ener > fEnergyThreshold) triggers++;
  }

  if(triggers>=fRequireN) complete_trig = true;

  // std::cout << "Returning True Trigger : " << complete_trig << std::endl;
  fTriggered = complete_trig;

  return complete_trig;
}

} // - namespace COSMIC

#include "SimpleTrigger.hh"

#include "G4Event.hh"

#include "db/DB.hh"
#include "db/DBLink.hh"
#include "analysis/VTrigger.hh"
#include "analysis/VProcessor.hh"
#include "analysis/Analysis.hh"

namespace COSMIC {

SimpleTrigger::SimpleTrigger(DBLink* tbl)
  : VTrigger(tbl->GetIndexName())
{
  std::cout << "TRG: Loading Simple Trigger : " << tbl->GetIndexName() << std::endl;

  // Get list of detector objects
  if (tbl->Has("processors")){
    SetupProcessors(tbl->GetVecS("processors"));
  }

  // Get thresholds
  fEnergyThreshold = -1.0;
  fTimeThreshold = -1.0;
  if (tbl->Has("energy_threshold")) fEnergyThreshold = tbl->GetD("energy_threshold") * MeV;
  if (tbl->Has("time_threshold"))   fEnergyThreshold = tbl->GetD("time_threshold") * ns;

}

SimpleTrigger::SimpleTrigger(std::string name,
                             std::vector<std::string> processors,
                             G4double ethresh, G4double tthresh)
  : VTrigger(name)
{
  SetupProcessors(processors);
  fEnergyThreshold = ethresh;
  fTimeThreshold = tthresh;
}

void SimpleTrigger::SetupProcessors(std::vector<std::string> det) {
  for (uint i = 0; i < det.size(); i++) {
    VProcessor* detobj = Analysis::Get()->GetProcessor(det[i]);
    fProcessors.push_back(detobj);
    std::cout << " --> Processor Input " << i << " : " << det[i] << std::endl;
  }
}

bool SimpleTrigger::ProcessTrigger(const G4Event* /*event*/) {

  // Trigger requires all detectors to be triggered within a certain time window
  bool complete_trig = true;
  for (uint i = 0; i < fProcessors.size(); i++) {

    bool trig = fProcessors[i]->HasInfo();

    if (fEnergyThreshold > 0.0) {
      G4double ener = fProcessors[i]->GetEnergy();
      if (ener < fEnergyThreshold) trig = false;
    }

    if (!trig) {
      complete_trig = false;
    }
  }
  
  // std::cout << "Returning True Trigger : " << complete_trig << std::endl;
  fTriggered = complete_trig;
  return complete_trig;
}

} // - namespace COSMIC

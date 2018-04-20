#ifndef __COSMIC_ANALYSIS_HH__
#define __COSMIC_ANALYSIS_HH__

// System Headers
// G4 Headers
#include "g4root.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "globals.hh"
// COSMIC Headers
#include "VFluxProcessor.hh"

namespace COSMIC {

// Forward Declarations
class VProcessor;
class VTrigger;

class VDetector;

/// Singleton Analysis Manager that handles all cosmic run processing
/// Contains pointers to all processors, and triggers so that it can
/// determine when events should be saved. Everything is based on the ROOT
/// analysis manager as the output file is quick to use.
class Analysis {
public:
  /// Simple Constructor
  Analysis();

  /// Destructor keeps all processors alive for now....
  ~Analysis();

  /// Singleton Access Function
  static inline Analysis *Get()
  { return fPrimary == 0 ? fPrimary = new Analysis : fPrimary; };

  /// Access G4 Analysis Manager
  inline G4AnalysisManager* GetG4AnalysisManager() { return fG4Manager; };

  /// Quick access function to ROOT manager
  static inline G4AnalysisManager* GetG4Manager() {
    return Get()->GetG4AnalysisManager();
  }

  /// Set upper limit on saved events in total (across runs)
  inline void SetEventsToSave(int s) {fSavedEventsLimit = s;};
  /// Set upper limit on generated events in total (across runs)
  inline void SetEventsToGenerate(int n) {fGeneratedEventsLimit = n;};


  /// Setup NTuple processor info at start of run
  void BeginOfRunAction(const G4Run* run);
  /// Write NTuple processor info at end of run
  void EndOfRunAction(const G4Run* run);


  /// Reset processors/triggers at the start of event
  void BeginOfEventAction();
  /// Main processing loop
  void ProcessEvent(const G4Event* event);
  /// Record event using NTuple Manager
  void RecordEvent();
  /// Reset all processors and triggers
  void ResetState();


  /// Add a new trigger to vector of triggers
  void RegisterTrigger(VTrigger* t);
  /// Check trigger combination. Default is to search
  /// for at least one trigger being true.
  bool IsTriggered();
  /// Set all trigger states back to default values
  void ResetTriggers();
  /// Get Trigger by ID
  VTrigger* GetTrigger(std::string id);


  /// Add a new processor to vector of processors
  void RegisterProcessor(VProcessor* p);
  /// Reset the state of all processors
  void ResetProcessors();
  /// Get Processor by ID
  VProcessor* GetProcessor(std::string id);


  /// Add a new detector to vector
  void RegisterDetector(VDetector* d);
  /// Reset the state of all detectors
  void ResetDetectors();
  /// Get the detector by its own ID
  VDetector* GetDetector(std::string id);


  /// Set the flux processor (special case)
  void SetFluxProcessor(VFluxProcessor* p);
  /// Get the flux processor
  inline VFluxProcessor* GetFluxProcessor(){return fFluxProcessor;};
  /// Set the exposure time
  void SetExposureTimeLimit(G4double d){ fExposureTimeLimit = d; };

  G4double GetExposureTime(){ return fFluxProcessor->GetExposureTime(); };

  void SetOutputTag(std::string s){ fOutputTag = s; };
  void SetOutputRun(int i){ fRunID = i; };
  void SetOutputSubRun(int i){ fSubRunID = i; };
  void IncrementSubRun() { fSubRunID++; };

  int GetNSavedEvents(){ return fSavedEvents; };
  
protected:

  static Analysis *fPrimary; ///< Singleton Object

  G4AnalysisManager* fG4Manager; ///< G4 Output Manager

  std::vector<VProcessor*> fProcessors; ///< Vector of all processing chunks
  std::vector<VTrigger*> fTriggers;     ///< Vector of all triggering chunks
  std::vector<VDetector*> fDetectors;   ///< Vector of all sensitive detectors

  VFluxProcessor* fFluxProcessor; ///< Flux processor special case. Can decide when to end run.

  int fSavedEvents;      ///< Number of saved events so far
  int fSavedEventsLimit; ///< Max limit on saved events

  int fGeneratedEvents;       ///< Number of gen. events so far
  int fGeneratedEventsLimit;  ///< Max limit on gen. events

  G4double fExposureTimeLimit; ///< Exposure Time max in s

  bool fNTuplesSetup; ///< Check Ntuples OKAY

  std::string fOutputTag;
  int fRunID;
  int fSubRunID;

};

} // - namespace COSMIC
#endif


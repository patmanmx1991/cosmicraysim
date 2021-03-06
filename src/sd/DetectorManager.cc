#include "DetectorManager.hh"

#include "G4LogicalVolume.hh"

#include "db/DBTable.hh"
#include "TrueMuonTracker.hh"
#include "SimpleScintillatorSD.hh"
#include "LongDriftSD.hh"
#include "chance/AWEDriftSD.hh"
#include "chance/BristolRPCSD.hh"
#include "TargetMuonInOutTracker.hh"
#include "NeutronSD.hh"
#include "TrueInTracker.hh"

namespace COSMIC {

VDetector* DetectorObjectFactory::CreateSD(DBTable tbl) {

  // Retrieve the table that matches this sensitive
  std::string type = tbl.GetS("type");

  // Now Search for different types
  if (type.compare("truemuon") == 0) return new TrueMuonTracker(tbl);
  else if (type.compare("scintillator") == 0) return new SimpleScintillatorSD(tbl);
  else if (type.compare("neutron") == 0) return new NeutronSD(tbl);
  else if (type.compare("longdrift") == 0) return new LongDriftSD(tbl);
  else if (type.compare("awedrift") == 0) return new AWEDriftSD(tbl);
  else if (type.compare("bristolrpc") == 0) return new BristolRPCSD(tbl);
  else if (type.compare("muoninouttracker") == 0) return new TargetMuonInOutTracker(tbl);
  else if (type.compare("trueinabsorber") == 0) return new TrueInAbsorber(tbl);

  // Check we didn't get to here
  std::cout << "Failed to Create New SD : '" << type << "'" << std::endl;
  throw;  
  return 0;
}

} // - namespace COSMIC

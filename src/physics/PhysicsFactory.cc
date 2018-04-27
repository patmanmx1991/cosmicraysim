#include "PhysicsFactory.hh"

#include "Shielding.hh"
#include "QGSP_BERT_HP.hh"
#include "db/DB.hh"
#include "DefaultPhysics.hh"
#include "G4PhysListFactory.hh"

namespace COSMIC {
G4VUserPhysicsList* PhysicsFactory::LoadPhysicsList() {
  DBTable table = DBNEW::Get()->GetTable("GLOBAL", "config");
  return LoadPhysicsList(table);
}

G4VUserPhysicsList* PhysicsFactory::LoadPhysicsList(DBTable table) {

  // Get physList factory but also have support for user defined physics....
  G4PhysListFactory *physListFactory = new G4PhysListFactory();
  G4VUserPhysicsList* physicsList;

  // Get physics list by horrible string comparison
  std::string physics = table.GetS("physics");
  if (physics.compare("shielding") == 0) physicsList =  new Shielding();
  else if (physics.compare("default") == 0) physicsList = new DefaultPhysics();
  else {
    physicsList = physListFactory->GetReferencePhysList(physics);
  }

  // If found just return good list
  if (physicsList) return physicsList;

  // if here then failed
  std::cout << "Could not find physics list : " << physics << std::endl;
  throw;
}

G4Region* PhysicsFactory::LoadRegion(DBTable table) {

  std::string indexname  = table.GetIndexName();
  std::string regionname = table.GetS("region");
  DBTable regtbl = DBNEW::Get()->GetTable("REGION", regionname);

  G4Region* reg = new G4Region(indexname + "_" + regionname);
  G4ProductionCuts* cuts = new G4ProductionCuts();

  // Get a default
  G4double defaultcut = 0.1 * nm;
  if (regtbl.Has("default_cut")) defaultcut = regtbl.GetG4D("default_cut");

  cuts->SetProductionCut( defaultcut, "gamma"   );
  cuts->SetProductionCut( defaultcut, "e-"      );
  cuts->SetProductionCut( defaultcut, "e+"      );
  cuts->SetProductionCut( defaultcut, "mu-"     );
  cuts->SetProductionCut( defaultcut, "mu+"     );
  cuts->SetProductionCut( defaultcut, "tau-"    );
  cuts->SetProductionCut( defaultcut, "tau+"    );
  cuts->SetProductionCut( defaultcut, "pi+"     );
  cuts->SetProductionCut( defaultcut, "pi-"     );
  cuts->SetProductionCut( defaultcut, "pi0"     );
  cuts->SetProductionCut( defaultcut, "proton"  );
  cuts->SetProductionCut( defaultcut, "neutron" );

  // Get list of cuts/info for this region
  if (regtbl.Has("gamma_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("gamma_cut"), "gamma" );

  if (regtbl.Has("electron_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("electron_cut"), "e-" );
  if (regtbl.Has("positron_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("positron_cut"), "e+" );
  if (regtbl.Has("muon_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("muon_cut"), "mu-" );
  if (regtbl.Has("antimuon_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("antimuon_cut"), "mu+" );
  if (regtbl.Has("tau_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("tau_cut"), "tau-" );
  if (regtbl.Has("antitau_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("antitau_cut"), "tau+" );

  if (regtbl.Has("piplus_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("piplus_cut"), "pi+" );
  if (regtbl.Has("piminus_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("piminus_cut"), "pi-" );
  if (regtbl.Has("pizero_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("pizero_cut"), "pi0" );

  if (regtbl.Has("proton_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("proton_cut"), "proton" );
  if (regtbl.Has("neutron_cut"))
    cuts->SetProductionCut( regtbl.GetG4D("neutron_cut"), "neutron" );

  reg->SetProductionCuts(cuts);

  return reg;
}


}

#include "MaterialManager.hh"

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
#include "globals.hh"
#include "G4Material.hh"
#include "G4NistElementBuilder.hh"
#include "G4NistMaterialBuilder.hh"
#include "G4Pow.hh"
#include "db/DB.hh"

#include "db/DBLink.hh"

namespace COSMIC {


G4Element* MaterialFactory::GetElement(std::string name) {

  G4NistManager* nist = G4NistManager::Instance();
  G4Element* mat;

  // Try to find by atomic symbol
  // nist->FindOrBuildMaterial(name);
  mat = nist->FindOrBuildElement(name);
  if (mat) return mat;

  // If not available we have to build the element
  DBLink* mattbl = DB::Get()->GetLink("ELEMENT", name);
  double atomicmass  = mattbl->GetD("atomic_mass") * g / mole;
  int atomicnumber   = mattbl->GetI("atomic_number");
  std::string symbol = mattbl->GetS("symbol");

  // Create material
  mat = new G4Element(name, symbol, atomicnumber, atomicmass );
  return mat;
}

G4Material* MaterialFactory::GetMaterial(std::string name) {

  G4NistManager* nist = G4NistManager::Instance();
  G4Material* mat;

  // First Check if its inside the nist manager
  mat = nist->FindOrBuildMaterial(name);
  if (mat) return mat;

  // If not, find the material inside custom database
  DBLink* mattbl = DB::Get()->GetLink("MATERIAL", name);

  std::vector<std::string> elements = mattbl->GetVecS("element_names");
  std::vector<int>         counts   = mattbl->GetVecI("element_counts");
  G4double density = mattbl->GetD("density") * g / cm3;

  // Create the material
  std::cout << "MAT: Creating : " << name << std::endl;
  mat = new G4Material(name, density, counts.size());
  for (uint j = 0; j < counts.size(); j++) {
    std::cout << "MAT: Adding Element : " << elements[j] << std::endl;
    G4Element* ele = GetElement(elements[j]);
    mat->AddElement( ele, counts[j] );
  }


  // Dump the Table of registered materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  return mat;
}

} // - namespace COSMIC





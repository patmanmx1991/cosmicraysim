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
namespace COSMIC {


G4Element* MaterialFactory::GetElement(std::string name) {

  G4NistManager* nist = G4NistManager::Instance();
  G4Element* mat;

  // Try to find by atomic symbol
  mat = nist->FindOrBuildElement(name);
  if (mat) return mat;

  // If not available we have to build the element
  DBTable mattbl = DB::Get()->GetTable("ELEMENT", name);
  double atomicmass  = mattbl.GetG4D("atomic_mass");
  int atomicnumber   = mattbl.GetD("atomic_number");
  std::string symbol = mattbl.GetS("symbol");

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
  DBTable mattbl = DB::Get()->GetTable("MATERIAL", name);

  std::vector<std::string> elements = mattbl.GetVecS("element_names");
  std::vector<double>         counts   = mattbl.GetVecD("element_counts");
  G4double density = mattbl.GetG4D("density");
  std::cout << "Material Density : " << density << std::endl;

  // Create the material
  std::cout << "MAT: Creating : " << name << std::endl;
  mat = new G4Material(name, density, counts.size());
  for (uint j = 0; j < counts.size(); j++) {
    std::cout << "MAT: Adding Element : " << elements[j] << std::endl;
    G4Element* ele = GetElement(elements[j]);
    mat->AddElement( ele, counts[j] );
  }
  std::cout << "MAT: Material Density : " << mat->GetDensity()*cm3/g << " g/cm3" << std::endl;

  // Dump the Table of registered materials
  // G4cout << *(G4Material::GetMaterialTable()) << G4endl;

  return mat;
}


G4VisAttributes* MaterialFactory::GetVisForMaterial(DBTable table){

  // Make new vis attributes
  G4VisAttributes* vis = NULL;

  // If this isn't already a material table, check if database has some defaults
  if (table.GetTableName().compare("MATERIAL") != 0 and table.Has("material")){
    std::string mat_name = table.GetS("material");
    vis = GetVisForMaterial(mat_name);
  }

  // Make if not already
  if (!vis) vis = new G4VisAttributes();

  // Update vis depending on whats in the table

  // Optional visualization parts
  if (table.Has("color")) {
    std::vector<double> color = table.GetVecD("color");
    if (color.size() == 3) // RGB
      vis->SetColour(G4Colour(color[0], color[1], color[2]));
    else if (color.size() == 4) // RGBA
      vis->SetColour(G4Colour(color[0], color[1], color[2], color[3]));
  }

  if (table.Has("drawstyle")) {
    std::string drawstyle = table.GetS("drawstyle");
    if (drawstyle == "wireframe")
      vis->SetForceWireframe(true);
    else if (drawstyle == "solid")
      vis->SetForceSolid(true);
    else throw;
  }

  if (table.Has("force_auxedge")) {
    int force_auxedge = table.GetI("force_auxedge");
    vis->SetForceAuxEdgeVisible(force_auxedge == 1);
  }

  return vis;  
}

G4VisAttributes* MaterialFactory::GetVisForMaterial(std::string name){

  /// Get the table for this material
  if (!DB::Get()->HasTable("MATERIAL",name)) return NULL;
  DBTable table = DB::Get()->GetTable("MATERIAL", name);

  /// Build vis from this table
  G4VisAttributes* vis = GetVisForMaterial(table);
  return vis;
}




} // - namespace COSMIC

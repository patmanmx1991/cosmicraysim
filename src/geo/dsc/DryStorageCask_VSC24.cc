#include "DryStorageCask_VSC24.hh"

#include "GeoObject.hh"
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
#include "GeoUtils.hh"
#include "sd/DetectorManager.hh"
#include "simple/GeoBox.hh"
#include "simple/GeoTubs.hh"

namespace COSMIC {

DryStorageCask_VSC24::DryStorageCask_VSC24(DBLink* table){
  Construct(table);
}

void DryStorageCask_VSC24::Construct(DBLink* table)
{
  std::cout << "Constructing Dry Storage Cask" << std::endl;
  SetID(table->GetIndexName());
  SetType(table->GetS("type"));

  std::string fName = table->GetIndexName();
  std::string fMotherName = table->GetS("mother");

  // Make a temporary DB from geo file
  DB* tdb = new DB("dsc/vsc24.geo");

  // Make the main logical volume (with mother and position/rotation overriden)
  DBLink* voltable = tdb->GetLink("GEO", "volume");
  voltable->SetIndexName(fName + "_volume");
  voltable->Set("mother", fMotherName);
  voltable->Set("position", table->GetVecD("position"));
  voltable->Set("rotation", table->GetVecD("rotation"));
  GeoObject* volume = new GeoTubs( voltable );
  fSubObjects.push_back(volume);
  fLogical = volume->GetLogical();
  fPhysical = volume->GetPhysical();
  fMotherLogical = volume->GetMotherLogical();

  // Now make the remaining objects inside this volume
  std::vector<std::string> tubs;
  tubs.push_back("concrete_cask");
  tubs.push_back("concrete_bottom");
  tubs.push_back("steel_shell");
  tubs.push_back("steel_top");
  tubs.push_back("steel_bottom");
  for (uint i = 0; i < tubs.size(); i++) {
    DBLink* tbl = tdb->GetLink("GEO", tubs[i]);
    tbl->SetIndexName(fName + "_" + tubs[i]);
    tbl->Prefix("mother", fName + "_");
    fSubObjects.push_back(new GeoTubs(tbl));
  }

  // Now Make the uranium rod array
  DBLink* rodpositions = tdb->GetLink("GEO", "rod_positions");

  std::vector<int> rodmask;
  if (table->Has("storage_mask")) rodmask = table->GetVecI("storage_mask");

  std::vector<int> rodmatindex;
  std::vector<std::string> radmatstring;
  if (table->Has("matrep_index")) rodmatindex = table->GetVecI("matrep_index");
  if (table->Has("matrep_material")) radmatstring = table->GetVecS("matrep_material");

  // Loop over possible positions and place rods
  for (int i = 0; i < 24; i++) {

    // Masking
    bool badrod = false;
    for (int j = 0; j < rodmask.size(); j++) {
      if (i == rodmask[j]) {
        badrod = true;
        break;
      }
    }
    if (badrod) continue;

    // Altered Materials
    std::string rodtype = "uranium_rod";
    for (int j = 0; j < rodmatindex.size(); j++) {
      if (i == rodmatindex[j]) {
        rodtype = radmatstring[j];
      }
    }

    // Get this rods position
    std::string rodname = "rod_" + std::to_string(i);
    std::vector<double> position = rodpositions->GetVecD(rodname);

    // Create the target rod by overloading table
    DBLink* rodtemplate  = tdb->CloneLink("GEO", rodtype, fName + "_" + rodname);
    rodtemplate->Prefix("mother", fName + "_");
    rodtemplate->Set("position", position);
    fSubObjects.push_back(new GeoBox(rodtemplate));

  }

  // Remove the temporary database.
  delete tdb;

}

} // namespace COSMIC
#include "geo/GeoManager.hh"
#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"

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

#include "GeoBox.hh"
#include "GeoTubs.hh"

///Task5d.1: Don't forget to include header file of your sensitive detector                                                                                                                                                                 
#include "db/DB.hh"
#include "db/DBTable.hh"
#include <map>

namespace COSMIC{


GeoObject* GeoObjectFactory::Construct(DBLink* table){

  std::string type = table->GetS("type");
  if (type.compare("box")==0) return new GeoBox(table);
  else if (type.compare("tubs")==0) return new GeoTubs(table);

  std::cout << "Failed to Construct Geomtry" << std::endl;
  return 0;
}






GeoManager *GeoManager::fPrimary(0);

GeoManager::GeoManager(){
  fGeoTables = DB::Get()->GetLinkGroup("GEO");
}

G4VPhysicalVolume* GeoManager::ConstructAll(){
  std::vector<DBLink*> tables_clone = fGeoTables;
  std::vector<DBLink*>::iterator geo_iter = tables_clone.begin();

  int count = 0;
  while (tables_clone.size() > 0){

    for (geo_iter = tables_clone.begin(); geo_iter != tables_clone.end();){

      DBLink* geo_tab = (*geo_iter);
      std::string geo_id = geo_tab->GetIndexName();

      // Check doesn't already exist
      if (HasGeoObject(geo_id)){
        std::cout << "Trying to add duplicate GEOM! " 
                  << "INDEX : " << geo_id << std::endl;
        geo_tab->Print();
        throw;
      }
        
      // Check if mother already created
      int mother_status = MotherStatus(geo_tab);
      std::cout << " : --> " << mother_status << std::endl;
      if (mother_status == 2){
        ++geo_iter;
        continue; // If it hasn't skip for now
      }

      GeoObject* geo_obj = GeoObjectFactory::Construct(geo_tab);
      fGeoObjects[geo_id] = geo_obj;
      fGeoIDs.push_back(geo_id);

      // Remove tabs
      geo_iter = tables_clone.erase(geo_iter);


    }
    if (count > 10000){
      std::cout << "Stuck in infinite detector loop" << std::endl;
      throw;
    }
  }

  return fGeoObjects["world"]->GetPhysical();
}


int GeoManager::MotherStatus(DBLink* geo_tab){
  if (geo_tab->Has("mother")){
    std::string mother = (geo_tab)->GetS("mother");
    std::cout << "Mother Status : " << geo_tab->GetIndexName() << " : " << mother << std::endl;

    if (this->HasGeoObject(mother)){
      return 1;
    } else{
      return 2;
    }
  }
  return 0; // World has no mother
}

bool GeoManager::HasGeoObject(std::string name){
  return (fGeoObjects.find(name) != fGeoObjects.end());
}


void GeoManager::ConstructSensitive(){
  for (int i = 0; i < fGeoIDs.size(); i++){
    fGeoObjects[fGeoIDs[i]]->ConstructSensitive();
  }
}

}
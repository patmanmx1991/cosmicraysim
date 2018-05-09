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

#include "simple/GeoBox.hh"
#include "simple/GeoTubs.hh"
#include "simple/GeoCone.hh"
#include "simple/GeoEllipticalTube.hh"
#include "dsc/DryStorageCask_VSC24.hh"
#include "awemuontom/AWEMuonTomographyDetector.hh"
#include "awemuontom/AWEMuonTomographyDetectorSecond.hh"
#include "hybrid/HybridMuonTomographyDetector.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"
#include <map>

namespace COSMIC{


GeoObject* GeoObjectFactory::Construct(DBTable table){

  std::string type = table.GetS("type");
  
  std::cout << "GEO: Constructing " << type << " : " << table.GetIndexName() <<  std::endl;
  if (type.compare("box")==0) return new GeoBox(table);
  else if (type.compare("tubs")==0) return new GeoTubs(table);
  else if (type.compare("cons")==0) return new GeoCone(table);
  else if (type.compare("eliptube")==0) return new GeoEllipticalTube(table);
  else if (type.compare("DSC_VSC24") == 0) return new DryStorageCask_VSC24(table);
  else if (type.compare("awe_muontom") == 0) return new AWEMuonTomographyDetector(table);
  else if (type.compare("awe_muontom2") == 0) return new AWEMuonTomographyDetectorSecond(table);
  else if (type.compare("hybrid_muontom") == 0) return new HybridMuonTomographyDetector(table);


  std::cout << "Failed to Construct Geometry" << std::endl;
  throw;
  return 0;
}





//----------------------------------------------------------
GeoManager *GeoManager::fPrimary(0);

GeoManager::GeoManager(){
}

G4VPhysicalVolume* GeoManager::ConstructAll(){


  fGeoObjects.clear();


  std::cout << "===============================" << std::endl;
  std::cout << "GEO: Building Geometry " << std::endl;
  std::vector<DBTable> tables_clone = DB::Get()->GetTableGroup("GEO");
  std::vector<DBTable>::iterator geo_iter = tables_clone.begin();

  int count = 0;
  while (tables_clone.size() > 0){

    for (geo_iter = tables_clone.begin(); geo_iter != tables_clone.end();){

      DBTable geo_tab = (*geo_iter);
      std::string geo_id = geo_tab.GetIndexName();

      // Check doesn't already exist
      if (HasGeoObject(geo_id)){
        std::cout << "Trying to add duplicate GEOM! " 
                  << "INDEX : " << geo_id << std::endl;
        geo_tab.Print();
        throw;
      }
        
      // Check if mother already created
      int mother_status = MotherStatus(geo_tab);
      if (mother_status == 2){
        count++;
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

  fWorldVolume = fGeoObjects["world"]->GetPhysical();
  return fWorldVolume;
}


int GeoManager::MotherStatus(DBTable geo_tab){
  if (geo_tab.Has("mother")){
    std::string mother = (geo_tab).GetS("mother");
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

}

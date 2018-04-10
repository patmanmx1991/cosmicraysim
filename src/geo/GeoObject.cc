#include "GeoObject.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

namespace COSMIC{

GeoObject::GeoObject(){
  fTable = NULL;
  fLogical = NULL;
  fPhysical = NULL;
  fMotherLogical = NULL;
  fMotherPhysical = NULL;
}

GeoObject::GeoObject(DBLink* _table){
  Construct( _table);
}

GeoObject::~GeoObject(){
  fTable = NULL;
  fLogical = NULL;
  fPhysical = NULL;
  fMotherLogical = NULL;
  fMotherPhysical = NULL;
}

// G4LogicalVolume* GeoObject::GetLogical(){
//   return fLogical;
// }

// G4VPhysicalVolume* GeoObject::GetPhysical(){
//   return fPhysical;
// }

// G4LogicalVolume* GeoObject::GetMotherLogical(){
//   return fMotherLogical;
// }

// G4PhysicalVolume* GeoObject::GetMotherPhysical(){
//   return fMotherPhysical;
// }



} // namespace COSMIC
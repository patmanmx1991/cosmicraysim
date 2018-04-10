#ifndef __GEO_OBJECT_HH__
#define __GEO_OBJECT_HH__

#include "db/DBLink.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

class GeoObject {
public:
  GeoObject();
  ~GeoObject();
  GeoObject(DBLink* _table);
  inline G4LogicalVolume* GetLogical() { return fLogical; }
  inline G4LogicalVolume* GetMotherLogical() { return fMotherLogical; }
  inline G4VPhysicalVolume* GetPhysical() { return fPhysical; }
  inline G4VPhysicalVolume* GetMotherPhysical() { return fMotherPhysical; }
  virtual void Construct(DBLink* /*table*/){};
  virtual void ConstructSensitive(){};

  DBLink* fTable;
  std::string fName;
  G4LogicalVolume* fLogical;
  G4VPhysicalVolume* fPhysical;
  G4LogicalVolume* fMotherLogical;
  G4VPhysicalVolume* fMotherPhysical;

};

} // namespace COSMIC
#endif

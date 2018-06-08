#ifndef __ISOSpacingTest_HH__
#define __ISOSpacingTest_HH__

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoSolid.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

/// Small steel drum geometry
class ISOSpacingTest : public GeoSolid {
public:

  /// Can only be constructed from a table
  ISOSpacingTest(DBTable table);

  /// Builds the geometry from some template DB file
  void Construct(DBTable table);  

};

} // namespace COSMIC
#endif

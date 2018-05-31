#ifndef __LargeSteelDrum_HH__
#define __LargeSteelDrum_HH__

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoSolid.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

/// Large steel drum geometry
class LargeSteelDrum : public GeoSolid {
public:

  /// Can only be constructed from a table
  LargeSteelDrum(DBTable table);

  /// Builds the geometry from some template DB file
  void Construct(DBTable table);  

};

} // namespace COSMIC
#endif

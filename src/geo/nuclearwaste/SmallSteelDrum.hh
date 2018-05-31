#ifndef __SmallSteelDrum_HH__
#define __SmallSteelDrum_HH__

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoObject.hh"
#include "geo/GeoSolid.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

/// Small steel drum geometry
class SmallSteelDrum : public GeoSolid {
public:

  /// Can only be constructed from a table
  SmallSteelDrum(DBTable table);

  /// Builds the geometry from some template DB file
  void Construct(DBTable table);  

};

} // namespace COSMIC
#endif

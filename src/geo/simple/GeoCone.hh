#ifndef __GEO_CONS__HH__
#define __GEO_CONS__HH__

#include "G4Cons.hh"

#include "db/DBTable.hh"
#include "geo/GeoSolid.hh"

namespace COSMIC {

/// Simple solid cone geometry object
class GeoCone : public GeoSolid {
public:
  /// Create a GeoCons from a table
  GeoCone(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace COSMIC
#endif


#ifndef __GEO_PARA_HH__
#define __GEO_PARA_HH__

#include "GeoSolid.hh"
#include "G4Para.hh"

#include "db/DBTable.hh"

namespace COSMIC {

/// Simple solid box geometry object
class GeoPara : public GeoSolid {
public:
  /// Create a GeoPara from a table
  GeoPara(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace GEOBOX
#endif


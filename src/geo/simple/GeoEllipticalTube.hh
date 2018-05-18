#ifndef __GEO_ELLIPTICALTUBE_HH__
#define __GEO_ELLIPTICALTUBE_HH__

#include "G4EllipticalTube.hh"

#include "db/DBTable.hh"
#include "geo/GeoSolid.hh"

namespace COSMIC {

/// Simple solid box geometry object
class GeoEllipticalTube : public GeoSolid {
public:
  /// Create a GeoBox from a table
  GeoEllipticalTube(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace GEOELLIPTICALTUBE
#endif


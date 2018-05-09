#ifndef __GEO_ELLIPTICALTUNNEL_HH__
#define __GEO_ELLIPTICALTUNNEL_HH__

#include "GeoSolid.hh"
// #include "G4EllipticalTunnel.hh"

#include "db/DBTable.hh"

namespace COSMIC {

/// Simple solid box geometry object
class GeoEllipticalTunnel : public GeoSolid {
public:
  /// Create a GeoBox from a table
  GeoEllipticalTunnel(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace GeoEllipticalTunnel
#endif

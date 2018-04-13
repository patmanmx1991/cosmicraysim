#ifndef __GEO_TUBS__HH__
#define __GEO_TUBS__HH__

#include "GeoSolid.hh"
#include "G4Tubs.hh"

class COSMIC::DBLink;

namespace COSMIC {

/// Simple solid tubs geometry object
class GeoTubs : public GeoSolid {
public:
  /// Create a GeoTubs from a table
  GeoTubs(DBLink* table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBLink* table);
};

} // namespace COSMIC
#endif


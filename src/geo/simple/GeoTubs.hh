#ifndef __GEO_TUBS__HH__
#define __GEO_TUBS__HH__

#include "GeoSolid.hh"
#include "G4Tubs.hh"

class COSMIC::DBTable;

namespace COSMIC {

/// Simple solid tubs geometry object
class GeoTubs : public GeoSolid {
public:
  /// Create a GeoTubs from a table
  GeoTubs(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace COSMIC
#endif


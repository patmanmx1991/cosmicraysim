#ifndef __GEO_CUTTUBS_HH__
#define __GEO_CUTTUBS_HH__

#include "GeoSolid.hh"
#include "G4CutTubs.hh"

#include "db/DBTable.hh"

namespace COSMIC {

/// Simple solid cut tubs geometry object
class GeoCutTubs : public GeoSolid {
public:
  /// Create a GeoCutTubs from a table
  GeoCutTubs(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace COSMIC
#endif


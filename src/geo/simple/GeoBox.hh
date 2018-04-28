#ifndef __GEO_BOX__HH__
#define __GEO_BOX__HH__

#include "GeoSolid.hh"
#include "G4Box.hh"

#include "db/DBTable.hh"

namespace COSMIC {

/// Simple solid box geometry object
class GeoBox : public GeoSolid {
public:
  /// Create a GeoBox from a table
  GeoBox(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace GEOBOX
#endif


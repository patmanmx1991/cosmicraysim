#ifdef __USE_CADMESH__
#ifndef __GEO_CADMESH__HH__
#define __GEO_CADMESH__HH__

#include "G4Box.hh"

#include "db/DBTable.hh"
#include "geo/GeoSolid.hh"

namespace COSMIC {

/// Simple solid box geometry object
class GeoCADMesh : public GeoSolid {
public:
  /// Create a GeoCADMesh from a table
  GeoCADMesh(DBTable table);

  /// Construct the solid volume for this geobox
  G4VSolid *ConstructSolidVolume(DBTable table);
};

} // namespace GEOBOX
#endif
#endif // __USE_CADMESH__

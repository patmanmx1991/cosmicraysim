#ifndef __DryStorageCask_VSC24_HH__
#define __DryStorageCask_VSC24_HH__

#include "db/DB.hh"
#include "db/DBTable.hh"
#include "geo/GeoObject.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

/// DSC Problem Geometry. Corresponds to approximation
/// of the dry storage cask used to store nuclear waste.
class DryStorageCask_VSC24 : public GeoObject {
public:

  /// Can only be constructed from a table
  DryStorageCask_VSC24(DBTable table);

  /// Builds the geometry from some template DB file
  void Construct(DBTable table);  

protected:

  std::vector<GeoObject*> fSubObjects; ///< Vector of all sub geometries
};

} // namespace COSMIC
#endif

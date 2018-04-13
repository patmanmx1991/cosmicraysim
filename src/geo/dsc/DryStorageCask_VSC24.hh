#ifndef __DryStorageCask_VSC24_HH__
#define __DryStorageCask_VSC24_HH__

#include "GeoObject.hh"
#include "db/DB.hh"
#include "db/DBLink.hh"

class G4LogicalVolume;
class G4VPhysicalVolume;

namespace COSMIC {

/// DSC Problem Geometry. Corresponds to approximation
/// of the dry storage cask used to store nuclear waste.
class DryStorageCask_VSC24 : public GeoObject {
public:

  /// Can only be constructed from a table
  DryStorageCask_VSC24(DBLink* table);

  /// Builds the geometry from some template DB file
  void Construct(DBLink* table);  

protected:

  std::vector<GeoObject*> fSubObjects; ///< Vector of all sub geometries
};

} // namespace COSMIC
#endif
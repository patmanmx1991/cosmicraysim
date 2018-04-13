#ifndef __DETECTOR_MANAGER_HH__
#define __DETECTOR_MANAGER_HH__

#include <iostream>

namespace COSMIC {

// Forward Declarations
class DBLink;
class VDetector;

/// Detector Factory used to create SD from tables
namespace DetectorObjectFactory {

/// Function to create detector objects from tables
VDetector* CreateSD(DBLink* tbl);

} // - namespace DetectorObjectFactory
} // - namespace COSMIC
#endif

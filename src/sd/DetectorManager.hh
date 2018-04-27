#ifndef __DETECTOR_MANAGER_HH__
#define __DETECTOR_MANAGER_HH__

#include <iostream>

namespace COSMIC {

// Forward Declarations
class DBTable;
class VDetector;

/// Detector Factory used to create SD from tables
namespace DetectorObjectFactory {

/// Function to create detector objects from tables
VDetector* CreateSD(DBTable tbl);

} // - namespace DetectorObjectFactory
} // - namespace COSMIC
#endif

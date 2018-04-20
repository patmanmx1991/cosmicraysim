#ifndef __MATERIALS_MANAGER_HH__
#define __MATERIALS_MANAGER_HH__

#include <iostream>

class G4Material;
class G4Element;


namespace COSMIC {

// Forward Declarations
class DBLink;
class VDetector;

/// Detector Factory used to create SD from tables
namespace MaterialFactory {

/// Get Element from string
G4Element* GetElement(std::string name);


/// Function to create detector objects from tables
G4Material* GetMaterial(std::string name);

} // - namespace DetectorObjectFactory
} // - namespace COSMIC
#endif


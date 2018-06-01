#ifndef __MATERIALS_MANAGER_HH__
#define __MATERIALS_MANAGER_HH__

#include <iostream>

#include "db/DBTable.hh"

class G4Material;
class G4Element;

namespace COSMIC {

/// Detector Factory used to create SD from tables
namespace MaterialFactory {

/// Get Element from string
G4Element* GetElement(std::string name);

/// Function to create detector objects from tables
G4Material* GetMaterial(std::string name);

/// Get some logical visualisation attributes depending on
/// material defaults
G4VisAttributes* GetVisForMaterial(DBTable table);

/// Get vis just based on material database name
G4VisAttributes* GetVisForMaterial(std::string name);


} // - namespace MaterialFactory
} // - namespace COSMIC
#endif


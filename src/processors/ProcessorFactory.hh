#ifndef __PROCESSOR_FACTORY_HH__
#define __PROCESSOR_FACTORY_HH__

#include <iostream>
#include "analysis/VProcessor.hh"

namespace COSMIC {

// Forward Declarations
class DBTable;
class VDetector;

/// Detector Factory used to create SD from tables
namespace ProcessorFactory {

/// Function to create detector objects from tables
VProcessor* Construct(DBTable tbl);

/// Function to generate all possible triggers loaded by the user tables
void ConstructProcessors();


} // - namespace PROCESSOR FACTORY
} // - namespace COSMIC
#endif

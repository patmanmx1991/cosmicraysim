#ifndef __TRIGGER_MANAGER_HH__
#define __TRIGGER_MANAGER_HH__

namespace COSMIC {

// Forward Declaration
class VTrigger;
class DBTable;

/// Globally accessible Factory for producing triggers.
namespace TriggerFactory {

/// Function to create trigger objects from tables
VTrigger* Construct(DBTable table);

/// Function to generate all possible triggers loaded by the user tables
void ConstructTriggers();

} // - namespace TriggerFactory
} // - namespace COSMIC
#endif

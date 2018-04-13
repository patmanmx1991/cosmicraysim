#ifndef __TRIGGER_MANAGER_HH__
#define __TRIGGER_MANAGER_HH__

namespace COSMIC {

// Forward Declaration
class VTrigger;
class DBLink;

/// Globally accessible Factory for producing triggers.
namespace TriggerFactory {

/// Function to create trigger objects from tables
VTrigger* Construct(DBLink* table);

} // - namespace TriggerFactory
} // - namespace COSMIC
#endif
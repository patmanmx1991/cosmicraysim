#include "TriggerManager.hh"

#include "db/DBLink.hh"
#include "analysis/VTrigger.hh"
#include "SimpleTrigger.hh"

namespace COSMIC {

VTrigger* TriggerFactory::Construct(DBLink* table) {

  /// Get the type from the table
  std::string type = table->GetS("type");

  /// Construct according to table settings
  if (type.compare("simple") == 0) return new SimpleTrigger(table);

  // Check we didn't get to here and fail string comparison
  std::cout << "Failed to Create Trigger : " << type << std::endl;
  throw;
  return 0;
}

} // - namespace COSMIC


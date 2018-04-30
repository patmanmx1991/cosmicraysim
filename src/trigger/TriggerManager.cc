#include "TriggerManager.hh"
#include "db/DB.hh"
#include "db/DBTable.hh"
#include "analysis/VTrigger.hh"
#include "analysis/Analysis.hh"

#include "SimpleTrigger.hh"

namespace COSMIC {

VTrigger* TriggerFactory::Construct(DBTable table) {

  /// Get the type from the table
  std::string type = table.GetS("type");

  /// Construct according to table settings
  if (type.compare("simple") == 0) return new SimpleTrigger(table);

  // Check we didn't get to here and fail string comparison
  std::cout << "Failed to Create Trigger : " << type << std::endl;
  throw;
  return 0;
}

void TriggerFactory::ConstructTriggers() {

  /// Get the DB Table Set
  std::cout << "===============================" << std::endl;
  std::cout << "TRG: Building Triggers " << std::endl;
  std::vector<DBTable> tables_clone = DB::Get()->GetTableGroup("TRIGGER");
  std::vector<DBTable>::iterator trg_iter = tables_clone.begin();

  for (trg_iter = tables_clone.begin(); trg_iter != tables_clone.end(); trg_iter++) {

    DBTable trg_tab = (*trg_iter);
    std::string trg_id = trg_tab.GetIndexName();

    // Create and register to analysis manager
    VTrigger* trg_obj = TriggerFactory::Construct(trg_tab);
    Analysis::Get()->RegisterTrigger(trg_obj);
  }

  return;
}

} // - namespace COSMIC


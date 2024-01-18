return ice_maker_slots({
  ice_maker_slot_0 = import("../IceMaker/ice_maker_slot_twist_tray_freezer_qa.lua"),
  ice_maker_slot_1 = import("../IceMaker/ice_maker_slot_aluminum_mold_freezer_qa.lua"),
  ice_maker_slot_2 = import("../IceMaker/ice_maker_slot_disabled.lua")
})

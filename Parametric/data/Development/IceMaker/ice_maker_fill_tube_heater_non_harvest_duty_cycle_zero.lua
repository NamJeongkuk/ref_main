return ice_maker({
  ice_maker_slots = import("../IceMaker/ice_maker_slots_non_harvest_fill_tube_duty_cycle_zero.lua"),
  ice_maker_fill_blocker = import("../IceMaker/ice_maker_fill_blocker.lua")
})

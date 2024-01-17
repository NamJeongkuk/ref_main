return ice_maker_slot({
  enable = true,
  type = ice_maker_type.twist_tray,
  location = ice_maker_location.freezer,
  ice_maker_data = import("../IceMaker/twist_tray_ice_maker.lua"),
  freezer_ice_rate = import("../IceMaker/freezer_ice_rate.lua"),
  non_harvest_fill_tube_heater = import("../IceMaker/non_harvest_fill_tube_heater.lua")
})

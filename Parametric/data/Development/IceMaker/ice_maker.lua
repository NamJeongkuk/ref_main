return ice_maker({
  freezer_ice_rate = import("../IceMaker/freezer_ice_rate.lua"),
  aluminum_mold_ice_maker = import("../IceMaker/aluminum_mold_ice_maker.lua"),
  twist_tray_ice_maker = import("../IceMaker/twist_tray_ice_maker.lua"),
  auger_motor = import("../IceMaker/auger_motor.lua")
})

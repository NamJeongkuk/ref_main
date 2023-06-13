return fan_list({
  condenser_fan = import("../Fan/condenser_fan.lua"),
  convertible_compartment_fan = import("../Fan/convertible_compartment_fan.lua"),
  ice_cabinet_fan = import("../Fan/ice_cabinet_fan.lua"),
  deli_fan = import("../Fan/deli_fan.lua"),
  freezer_evap_fan = import("../Fan/freezer_evap_fan.lua"),
  fresh_food_evap_fan = import("../Fan/fresh_food_evap_fan.lua")
})

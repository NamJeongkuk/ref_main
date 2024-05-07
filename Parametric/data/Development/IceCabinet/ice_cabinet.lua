return ice_cabinet({
  gamma_table = import('../IceCabinet/gamma_table.lua'),
  ice_cabinet_fan_startup = import('../IceCabinet/ice_cabinet_fan_startup.lua'),
  freezer_setpoint_to_ice_cabinet_setpoint_table = import('../IceCabinet/freezer_setpoint_to_ice_cabinet_setpoint_table.lua')
})

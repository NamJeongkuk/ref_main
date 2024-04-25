return ice_cabinet_adjusted_setpoint({
  ice_formation_offset_in_degfx100 = -200,
  cross_ambient_offset = import('../IceCabinet/ice_cabinet_cross_ambient_offset.lua'),
  shift_offset = import('../IceCabinet/ice_cabinet_shift_offset.lua')
})

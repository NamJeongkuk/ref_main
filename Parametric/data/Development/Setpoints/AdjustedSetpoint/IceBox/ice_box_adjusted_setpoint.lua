return ice_box_adjusted_setpoint({
  ice_box_offset_in_degfx100 = -100,
  ice_formation_offset_in_degfx100 = -200,
  shift_offset = import('../IceBox/ice_box_shift_offset.lua')
})

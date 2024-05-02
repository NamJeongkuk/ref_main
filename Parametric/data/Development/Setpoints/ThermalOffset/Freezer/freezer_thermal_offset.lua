return freezer_thermal_offset({
  cabinet_offset_in_degfx100 = - 10,
  cross_ambient_offset = import('../Freezer/freezer_cross_ambient_offset.lua'),
  high_ambient_offset = import('../Freezer/freezer_high_ambient_offset.lua'),
  shift_offset = import('../Freezer/freezer_shift_offset.lua')
})

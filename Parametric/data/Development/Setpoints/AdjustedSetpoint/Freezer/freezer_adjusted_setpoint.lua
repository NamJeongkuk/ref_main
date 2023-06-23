return freezer_adjusted_setpoint({
  cross_ambient_offset = import('../Freezer/freezer_cross_ambient_offset.lua'),
  high_ambient_offset = import('../Freezer/freezer_high_ambient_offset.lua'),
  shift_offset = import('../Freezer/freezer_shift_offset.lua')
})

return fresh_food_thermal_offset({
  cabinet_offset_in_degfx100 = 10,
  fresh_food_pulldown_offset_in_degfx100 = 0,
  cross_ambient_offset = import('../FreshFood/fresh_food_cross_ambient_offset.lua'),
  high_ambient_offset = import('../FreshFood/fresh_food_high_ambient_offset.lua'),
  setpoint_offset = import('../FreshFood/fresh_food_setpoint_offset.lua'),
  shift_offset = import('../FreshFood/fresh_food_shift_offset.lua')
})

return fresh_food_adjusted_setpoint({
  fresh_food_pulldown_offset_in_degfx100 = 30,
  cross_ambient_offset = import('../FreshFood/fresh_food_cross_ambient_offset.lua'),
  high_ambient_offset = import('../FreshFood/fresh_food_high_ambient_offset.lua'),
  setpoint_offset = import('../FreshFood/fresh_food_setpoint_offset.lua'),
  shift_offset = import('../FreshFood/fresh_food_shift_offset.lua')
})

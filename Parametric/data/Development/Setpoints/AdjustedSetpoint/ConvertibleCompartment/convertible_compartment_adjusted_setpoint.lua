return convertible_compartment_adjusted_setpoint({
  cross_ambient_offset_as_fresh_food = import('../ConvertibleCompartment/convertible_compartment_as_fresh_food_cross_ambient_offset.lua'),
  cross_ambient_offset_as_freezer = import('../ConvertibleCompartment/convertible_compartment_as_freezer_cross_ambient_offset.lua'),
  fixed_setpoint_offset = import('../ConvertibleCompartment/convertible_compartment_fixed_setpoint_offset.lua'),
  shift_offset = import('../ConvertibleCompartment/convertible_compartment_shift_offset.lua')
})

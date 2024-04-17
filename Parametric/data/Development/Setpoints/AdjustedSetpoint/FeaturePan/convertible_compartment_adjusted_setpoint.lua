return feature_pan_adjusted_setpoint({
  cross_ambient_offset_as_fresh_food = import('../FeaturePan/convertible_compartment_as_fresh_food_cross_ambient_offset.lua'),
  cross_ambient_offset_as_freezer = import('../FeaturePan/convertible_compartment_as_freezer_cross_ambient_offset.lua'),
  setpoint_offset = import('../FeaturePan/feature_pan_setpoint_offset.lua'),
  shift_offset = import('../FeaturePan/convertible_compartment_shift_offset.lua')
})

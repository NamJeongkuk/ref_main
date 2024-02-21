return adjusted_setpoint({
  fresh_food_adjusted_setpoint = import('../AdjustedSetpoint/FreshFood/fresh_food_adjusted_setpoint.lua'),
  freezer_adjusted_setpoint = import('../AdjustedSetpoint/Freezer/freezer_adjusted_setpoint.lua'),
  convertible_compartment_adjusted_setpoint = import('../AdjustedSetpoint/ConvertibleCompartment/convertible_compartment_adjusted_setpoint.lua'),
  deli_pan_adjusted_setpoint = import('../AdjustedSetpoint/DeliPan/deli_pan_adjusted_setpoint.lua'),
  ice_cabinet_adjusted_setpoint = import('../AdjustedSetpoint/IceCabinet/ice_cabinet_adjusted_setpoint.lua'),
  shift_offset_calculator = import('../AdjustedSetpoint/shift_offset_calculator.lua')
})

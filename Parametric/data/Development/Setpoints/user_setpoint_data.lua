return user_setpoint_data({
  freezer_user_setpoint = import("../Setpoints/freezer_user_setpoint.lua"),
  fresh_food_user_setpoint = import("../Setpoints/fresh_food_user_setpoint.lua"),
  feature_pan_user_setpoints = import("../Setpoints/feature_pan_user_setpoints.lua")
})

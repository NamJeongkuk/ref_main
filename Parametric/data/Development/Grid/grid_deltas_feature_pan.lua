local feature_pan_grid_deltas_mode_1 = import("../Grid/feature_pan_grid_deltas_mode_1.lua")
local feature_pan_grid_deltas_mode_2 = import("../Grid/feature_pan_grid_deltas_mode_2.lua")
local feature_pan_grid_deltas_mode_3 = import("../Grid/feature_pan_grid_deltas_mode_3.lua")
local feature_pan_grid_deltas_mode_4 = import("../Grid/feature_pan_grid_deltas_mode_4.lua")
local feature_pan_grid_deltas_mode_5 = import("../Grid/feature_pan_grid_deltas_mode_5.lua")

return grid_deltas({
  deltas = {
    grid_deltas = {
      mode_1 = feature_pan_grid_deltas_mode_1,
      mode_2 = feature_pan_grid_deltas_mode_2,
      mode_3 = feature_pan_grid_deltas_mode_3,
      mode_4 = feature_pan_grid_deltas_mode_4,
      mode_5 = feature_pan_grid_deltas_mode_5,
      mode_6 = feature_pan_grid_deltas_mode_1,
      mode_7 = feature_pan_grid_deltas_mode_1
    }
  }
})

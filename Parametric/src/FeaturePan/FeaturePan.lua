local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local fan_speed_type = require 'Fan.FanSpeedType'
local enum = require 'lua-common'.utilities.enum
local feature_pan_mode = require 'FeaturePan.FeaturePanMode'

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'feature_pan' },
      structure(
        i16(config.feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute),
        i16(config.feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute),
        i16(config.feature_pan_heater_high_ambient_temperature_limit_degFx100),
        i16(config.feature_pan_pulldown_temperature_limit_in_degfx100),
        u16(fan_speed_type[config.fresh_food_fan_deli_cooling_speed]),
        u8(config.feature_pan_default_mode),
        u8(config.feature_pan_min_allowed_mode),
        u8(config.feature_pan_max_allowed_mode)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        feature_pan_warmup_slope_lower_limit_in_degfx100_per_minute = { constraint.i16 },
        feature_pan_warmup_slope_upper_limit_in_degfx100_per_minute = { constraint.i16 },
        feature_pan_heater_high_ambient_temperature_limit_degFx100 = { constraint.i16 },
        feature_pan_pulldown_temperature_limit_in_degfx100 = { constraint.i16 },
        fresh_food_fan_deli_cooling_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
        feature_pan_default_mode = {
          constraint.in_enumeration(feature_pan_mode),
          constraint.in_range(config.feature_pan_min_allowed_mode, config.feature_pan_max_allowed_mode)
        },
        feature_pan_min_allowed_mode = { constraint.in_enumeration(feature_pan_mode) },
        feature_pan_max_allowed_mode = { constraint.in_enumeration(feature_pan_mode) }
      })
    return generate(config)
  end
end

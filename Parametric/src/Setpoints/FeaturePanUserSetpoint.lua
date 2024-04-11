local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local feature_pan_mode_label = require 'FeaturePan.FeaturePanModeLabel'

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'feature_pan_user_setpoint' },
      structure(
        i16(config.min_temperature_setpoint_in_degfx100),
        i16(config.max_temperature_setpoint_in_degfx100),
        i16(config.default_temperature_setpoint_in_degfx100),
        u8(config.mode_label),
        u8(0) -- reserved byte for public erd
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        max_temperature_setpoint_in_degfx100 = { constraint.i16 },
        min_temperature_setpoint_in_degfx100 = { constraint.i16 },
        default_temperature_setpoint_in_degfx100 = { constraint.in_range(config.min_temperature_setpoint_in_degfx100, config.max_temperature_setpoint_in_degfx100) },
        mode_label = { constraint.in_enumeration(feature_pan_mode_label) },
      })
    return generate(config)
  end
end

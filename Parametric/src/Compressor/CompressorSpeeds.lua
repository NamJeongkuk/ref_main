local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    if (config.cooling_mode_dependent ==  false) then
      return TypedString(
        { 'compressor_speeds' },
        structure(
          bool(config.cooling_mode_dependent),
          u8(config.startup_speed_frequency_in_hz),
          u8(config.super_low_speed_frequency_in_hz),
          u8(config.super_high_speed_frequency_in_hz),
          config.cooling_mode_independent_compressor_speeds
        )
      )
    else
      return TypedString(
        { 'compressor_speeds' },
        structure(
          bool(config.cooling_mode_dependent),
          u8(config.startup_speed_frequency_in_hz),
          u8(config.super_low_speed_frequency_in_hz),
          u8(config.super_high_speed_frequency_in_hz),
          config.cooling_mode_dependent_compressor_speeds
        )
      )
    end
  end)

  return function(config)
    if (config.cooling_mode_dependent == false) then
      validate_arguments(
        config,
        {
          cooling_mode_dependent = { constraint.boolean },
          startup_speed_frequency_in_hz = { constraint.u8 },
          super_low_speed_frequency_in_hz = { constraint.u8 },
          super_high_speed_frequency_in_hz = { constraint.u8 },
          cooling_mode_independent_compressor_speeds = { constraint.typed_string('cooling_mode_independent_compressor_speeds') }
        }
      )
    else
      validate_arguments(
        config,
        {
          cooling_mode_dependent = { constraint.boolean },
          startup_speed_frequency_in_hz = { constraint.u8 },
          super_low_speed_frequency_in_hz = { constraint.u8 },
          super_high_speed_frequency_in_hz = { constraint.u8 },
          cooling_mode_dependent_compressor_speeds = { constraint.typed_string('cooling_mode_dependent_compressor_speeds') }
        }
      )
    end
    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local variable_speed_filler = false;

return function(core)
  import(core)

  local generate = memoize(function(config)
    if config.compressor_is_single_speed == false then
      return TypedString(
        { 'compressor' },
        structure(
          bool(config.compressor_is_single_speed),
          i16(config.low_ambient_valve_trigger_temperature_in_deg_fx100),
          bool(config.variable_speed_compressor_relay_is_enabled),
          config.compressor_times,
          config.compressor_speeds
        )
      )
    else
      return TypedString(
        { 'compressor' },
        structure(
          bool(config.compressor_is_single_speed),
          i16(config.low_ambient_valve_trigger_temperature_in_deg_fx100),
          bool(variable_speed_filler),
          config.compressor_times
        )
      )
    end
  end)

  return function(config)
    if config.compressor_is_single_speed == false then
      validate_arguments(
        config,
        {
          compressor_is_single_speed = { constraint.boolean },
          low_ambient_valve_trigger_temperature_in_deg_fx100 = { constraint.i16 },
          variable_speed_compressor_relay_is_enabled = { constraint.boolean },
          compressor_times = { constraint.typed_string('compressor_times') },
          compressor_speeds = { constraint.typed_string('compressor_speeds') }
        })
    else
      validate_arguments(
        config,
        {
          compressor_is_single_speed = { constraint.boolean },
          low_ambient_valve_trigger_temperature_in_deg_fx100 = { constraint.i16 },
          compressor_times = { constraint.typed_string('compressor_times') }
        })
    end
    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'compressor_times' },
      structure(
        u8(config.minimum_off_time_in_minutes),
        u8(config.minimum_on_time_in_minutes),
        u8(config.startup_on_time_in_seconds),
        u8(config.remain_off_after_valve_move_in_minutes),
        u8(config.remain_off_after_valve_move_low_ambient_in_minutes),
        u8(config.variable_speed_minimum_run_time_in_minutes),
        u8(config.sabbath_delay_time_in_seconds),
        u8(config.compressor_off_cooling_on_max_allowed_time_in_minutes),
        u16(config.excessive_runtime_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        minimum_off_time_in_minutes = { constraint.u8 },
        minimum_on_time_in_minutes = { constraint.u8 },
        startup_on_time_in_seconds = { constraint.u8 },
        remain_off_after_valve_move_in_minutes = { constraint.u8 },
        remain_off_after_valve_move_low_ambient_in_minutes = { constraint.u8 },
        variable_speed_minimum_run_time_in_minutes = { constraint.u8 },
        sabbath_delay_time_in_seconds = { constraint.in_range(15, 25) },
        compressor_off_cooling_on_max_allowed_time_in_minutes = { constraint.u8 },
        excessive_runtime_in_minutes = { constraint.u16 }
      }
    )
    return generate(config)
  end
end

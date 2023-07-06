local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'harvest_count_calculator' },
      structure(
        i16(config.start_integration_temperature_in_deg_fx100),
        i16(config.minimum_freeze_time_initiation_temperature_in_deg_fx100),
        u32(config.target_integration_limit_in_deg_fx100_times_seconds),
        u8(config.minimum_freeze_time_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        start_integration_temperature_in_deg_fx100 = { constraint.i16 },
        minimum_freeze_time_initiation_temperature_in_deg_fx100 = { constraint.i16 },
        target_integration_limit_in_deg_fx100_times_seconds = { constraint.u32 },
        minimum_freeze_time_in_minutes = { constraint.u8 }
      }
    )
    return generate(config)
  end
end

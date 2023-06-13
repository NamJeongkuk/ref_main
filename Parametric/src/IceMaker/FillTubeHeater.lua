local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'fill_tube_heater' },
      structure(
        u8(config.freeze_thaw_fill_tube_heater_duty_cycle_percentage),
        u16(config.freeze_thaw_fill_tube_heater_on_time_in_seconds),
        u8(config.non_harvest_fill_tube_heater_duty_cycle_percentage)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = { constraint.in_range(0,100) },
        freeze_thaw_fill_tube_heater_on_time_in_seconds = { constraint.u16 },
        non_harvest_fill_tube_heater_duty_cycle_percentage = { constraint.in_range(0,100) }
      }
    )

    if (config.freeze_thaw_fill_tube_heater_duty_cycle_percentage == 0 and config.freeze_thaw_fill_tube_heater_on_time_in_seconds ~= 0) then
      error('if freeze_thaw_fill_tube_heater_duty_cycle_percentage is 0, then freeze_thaw_fill_tube_heater_on_time_in_seconds must be 0', 2)
    end

    if (config.freeze_thaw_fill_tube_heater_on_time_in_seconds == 0 and config.freeze_thaw_fill_tube_heater_duty_cycle_percentage ~= 0) then
      error('if freeze_thaw_fill_tube_heater_on_time_in_seconds is 0, then freeze_thaw_fill_tube_heater_duty_cycle_percentage must be 0', 2)
    end

    return generate(config)
  end
end

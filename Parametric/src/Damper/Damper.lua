local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
   import(core)
   local generate = memoize(function(config)
       return TypedString(
         { 'damper' },
         structure(
           u16(config.steps_to_open),
           u16(config.steps_to_close),
           u16(config.steps_to_home),
           u8(config.delay_between_step_events_in_hundreds_of_microseconds),
           u8(config.max_time_for_damper_to_be_open_before_cycling_in_minutes),
           u8(config.target_compartment_minimum_temperature_change_time_in_minutes),
           u8(config.target_compartment_damper_heater_on_time_in_minutes),
           i16(config.source_compartment_maximum_temperature_to_run_check_in_degfx100),
           i16(config.target_compartment_minimum_temperature_change_in_degfx100)
         )
       )
   end)

   return function(config)
      validate_arguments(
        config,
        {
          steps_to_open = { constraint.u16 },
          steps_to_close = { constraint.u16 },
          steps_to_home = { constraint.u16 },
          delay_between_step_events_in_hundreds_of_microseconds = { constraint.u8 },
          max_time_for_damper_to_be_open_before_cycling_in_minutes = { constraint.u8 },
          target_compartment_minimum_temperature_change_time_in_minutes = { constraint.u8 },
          target_compartment_damper_heater_on_time_in_minutes = { constraint.u8 },
          source_compartment_maximum_temperature_to_run_check_in_degfx100 = { constraint.i16 },
          target_compartment_minimum_temperature_change_in_degfx100 = { constraint.i16 }
        }
      )
      return generate(config)
    end
end

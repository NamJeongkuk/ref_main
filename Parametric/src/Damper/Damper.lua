local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local damper_id = require 'Damper/DamperId'

return function(core)
   import(core)
   local generate = memoize(function(config)
       return TypedString(
         { 'damper' },
         structure(
           u8(config.damper_id),
           pointer(config.damper_step_data),
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
      if config.damper_id == damper_id.normal_damper then
        validate_arguments(
          config,
          {
            damper_id = { constraint.in_enumeration(damper_id) },
            damper_step_data = { constraint.typed_string('damper_steps') },
            steps_to_home = { constraint.u16 },
            delay_between_step_events_in_hundreds_of_microseconds = { constraint.u8 },
            max_time_for_damper_to_be_open_before_cycling_in_minutes = { constraint.u8 },
            target_compartment_minimum_temperature_change_time_in_minutes = { constraint.u8 },
            target_compartment_damper_heater_on_time_in_minutes = { constraint.u8 },
            source_compartment_maximum_temperature_to_run_check_in_degfx100 = { constraint.i16 },
            target_compartment_minimum_temperature_change_in_degfx100 = { constraint.i16 }
          }
        )
      elseif config.damper_id == damper_id.convertible_compartment_damper then
        validate_arguments(
          config,
          {
            damper_id = { constraint.in_enumeration(damper_id) },
            damper_step_data = { constraint.typed_string('convertible_damper_step_data') },
            steps_to_home = { constraint.u16 },
            delay_between_step_events_in_hundreds_of_microseconds = { constraint.u8 },
            max_time_for_damper_to_be_open_before_cycling_in_minutes = { constraint.u8 },
            target_compartment_minimum_temperature_change_time_in_minutes = { constraint.u8 },
            target_compartment_damper_heater_on_time_in_minutes = { constraint.u8 },
            source_compartment_maximum_temperature_to_run_check_in_degfx100 = { constraint.i16 },
            target_compartment_minimum_temperature_change_in_degfx100 = { constraint.i16 }
          })
      else
        validate_arguments(
          config,
          {
            damper_id = { constraint.in_enumeration(damper_id) },
            damper_step_data = { constraint.ored({ constraint.typed_string('damper_steps'), constraint.typed_string('convertible_damper_step_data') }) },
            steps_to_home = { constraint.u16 },
            delay_between_step_events_in_hundreds_of_microseconds = { constraint.u8 },
            max_time_for_damper_to_be_open_before_cycling_in_minutes = { constraint.u8 },
            target_compartment_minimum_temperature_change_time_in_minutes = { constraint.u8 },
            target_compartment_damper_heater_on_time_in_minutes = { constraint.u8 },
            source_compartment_maximum_temperature_to_run_check_in_degfx100 = { constraint.i16 },
            target_compartment_minimum_temperature_change_in_degfx100 = { constraint.i16 }
          })
      end
      return generate(config)
    end
end

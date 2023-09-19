local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local sealed_system_valve_position_type = require 'SealedSystemValve.SealedSystemValvePositionType'
local damper_position_type = require 'Damper.DamperPositionType'
local fan_speed_type = require 'Fan.FanSpeedType'
local compressor_speed_type = require 'Compressor.CompressorSpeedType'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'defrost' },
      structure(
        structure(
          u16(config.idle.freezer_door_increment_factor_in_seconds_per_second),
          u16(config.idle.fresh_food_door_increment_factor_in_seconds_per_second),
          u16(config.idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes),
          u16(config.idle.max_time_between_defrosts_in_minutes),
          u16(config.idle.aham_prechill_time_between_defrosts_in_minutes)
        ),
        structure(
          u8(config.prechill_prep.number_of_fresh_food_defrosts_before_freezer_defrost),
          u8(config.prechill_prep.number_of_fresh_food_defrosts_before_abnormal_freezer_defrost),
          u8(config.prechill_prep.max_prechill_prep_time_in_minutes)
        ),
        structure(
          u8(config.prechill.max_prechill_time_in_minutes),
          u8(config.prechill.max_prechill_time_for_fresh_food_only_defrost_in_minutes),
          u8(sealed_system_valve_position_type[config.prechill.prechill_refrigerant_valve_position]),
          u8(compressor_speed_type[config.prechill.prechill_compressor_speed]),
          u8(fan_speed_type[config.prechill.prechill_freezer_evap_fan_speed]),
          u8(fan_speed_type[config.prechill.prechill_fresh_food_evap_fan_speed]),
          u8(fan_speed_type[config.prechill.prechill_ice_box_fan_speed]),
          u8(fan_speed_type[config.prechill.prechill_deli_fan_speed]),
          u8(damper_position_type[config.prechill.prechill_convertible_compartment_damper_position]),
          u8(fan_speed_type[config.prechill.prechill_convertible_compartment_fan_speed]),
          u8(damper_position_type[config.prechill.prechill_fresh_food_damper_position]),
          i16(config.prechill.prechill_freezer_min_temp_in_degfx100),
          i16(config.prechill.prechill_fresh_food_min_temp_in_degfx100),
          i16(config.prechill.prechill_fresh_food_max_temp_in_degfx100),
          i16(config.prechill.prechill_convertible_compartment_as_freezer_min_temp_in_degfx100),
          i16(config.prechill.prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100),
          i16(config.prechill.prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100),
          i16(config.prechill.prechill_freezer_evap_exit_temperature_in_degfx100),
          i16(config.prechill.prechill_convertible_compartment_evap_exit_temperature_in_degfx100)
        ),
        structure(
          u8(config.heater_on_entry.defrost_heater_on_delay_after_compressor_off_in_seconds),
          u8(damper_position_type[config.heater_on_entry.heater_on_entry_fresh_food_damper_position])
        ),
        structure(
          u8(sealed_system_valve_position_type[config.heater_on.defrost_heater_on_refrigerant_valve_position]),
          u8(config.heater_on.freezer_defrost_heater_max_on_time_in_minutes),
          u8(config.heater_on.freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes),
          u8(config.heater_on.freezer_heater_on_time_to_set_abnormal_defrost_in_minutes),
          i16(config.heater_on.freezer_defrost_termination_temperature_in_degfx100),
          i16(config.heater_on.fresh_food_defrost_termination_temperature_in_degfx100),
          i16(config.heater_on.convertible_compartment_defrost_termination_temperature_in_degfx100),
          u8(config.heater_on.fresh_food_defrost_heater_max_on_time_in_minutes),
          u8(config.heater_on.fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes),
          u8(config.heater_on.fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes),
          u8(config.heater_on.convertible_compartment_defrost_heater_max_on_time_in_minutes),
          u8(config.heater_on.convertible_compartment_as_fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes),
          u8(config.heater_on.convertible_compartment_as_freezer_heater_on_time_to_set_abnormal_defrost_in_minutes),
          u8(config.heater_on.convertible_compartment_as_fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes),
          u8(config.heater_on.convertible_compartment_as_freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes)
        ),
        structure(
          u8(config.dwell.dwell_time_in_minutes),
          u8(damper_position_type[config.dwell.dwell_fresh_food_damper_position]),
          u8(sealed_system_valve_position_type[config.dwell.dwell_refrigerant_valve_position])
        ),
        structure(
          u8(config.post_dwell.post_dwell_exit_time_in_minutes),
          u8(config.post_dwell.fresh_food_only_post_dwell_exit_time_in_minutes),
          i16(config.post_dwell.post_dwell_freezer_evap_exit_temperature_in_degfx100),
          i16(config.post_dwell.fresh_food_only_post_dwell_freezer_evap_exit_temperature_in_degfx100),
          u8(sealed_system_valve_position_type[config.post_dwell.post_dwell_refrigerant_valve_position]),
          u8(compressor_speed_type[config.post_dwell.post_dwell_compressor_speed]),
          u8(fan_speed_type[config.post_dwell.post_dwell_condenser_fan_speed]),
          u8(damper_position_type[config.post_dwell.post_dwell_fresh_food_damper_position])
        )
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        idle = {
          constraint.table_keys({
            freezer_door_increment_factor_in_seconds_per_second = { constraint.u16 },
            fresh_food_door_increment_factor_in_seconds_per_second = { constraint.u16 },
            minimum_time_between_defrosts_abnormal_run_time_in_minutes = { constraint.u16 },
            max_time_between_defrosts_in_minutes = { constraint.u16 },
            aham_prechill_time_between_defrosts_in_minutes = { constraint.u16 }
          })
        },
        prechill_prep = {
          constraint.table_keys({
            number_of_fresh_food_defrosts_before_freezer_defrost = { constraint.u8 },
            number_of_fresh_food_defrosts_before_abnormal_freezer_defrost = { constraint.u8 },
            max_prechill_prep_time_in_minutes = { constraint.u8 }
          })
        },
        prechill = {
          constraint.table_keys({
            max_prechill_time_in_minutes = { constraint.u8 },
            max_prechill_time_for_fresh_food_only_defrost_in_minutes = { constraint.u8 },
            prechill_refrigerant_valve_position = { constraint.in_set(enum.keys(sealed_system_valve_position_type)) },
            prechill_compressor_speed = { constraint.in_set(enum.keys(compressor_speed_type)) },
            prechill_freezer_evap_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
            prechill_fresh_food_evap_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
            prechill_ice_box_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
            prechill_deli_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
            prechill_convertible_compartment_damper_position = { constraint.in_set(enum.keys(damper_position_type)) },
            prechill_convertible_compartment_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
            prechill_fresh_food_damper_position = { constraint.in_set(enum.keys(damper_position_type)) },
            prechill_freezer_min_temp_in_degfx100 = { constraint.i16 },
            prechill_fresh_food_min_temp_in_degfx100 = { constraint.i16 },
            prechill_fresh_food_max_temp_in_degfx100 = { constraint.i16 },
            prechill_convertible_compartment_as_freezer_min_temp_in_degfx100 = { constraint.i16 },
            prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100 = { constraint.i16 },
            prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100 = { constraint.i16 },
            prechill_freezer_evap_exit_temperature_in_degfx100 = { constraint.i16 },
            prechill_convertible_compartment_evap_exit_temperature_in_degfx100 = { constraint.i16 }
          })
        },
        heater_on_entry = {
          constraint.table_keys({
            defrost_heater_on_delay_after_compressor_off_in_seconds = { constraint.u8 },
            heater_on_entry_fresh_food_damper_position = { constraint.in_set(enum.keys(damper_position_type)) }
          })
        },
        heater_on = {
          constraint.table_keys({
            defrost_heater_on_refrigerant_valve_position = { constraint.in_set(enum.keys(sealed_system_valve_position_type)) },
            freezer_defrost_heater_max_on_time_in_minutes = { constraint.u8 },
            freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = { constraint.u8 },
            freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = { constraint.u8 },
            freezer_defrost_termination_temperature_in_degfx100 = { constraint.i16 },
            fresh_food_defrost_termination_temperature_in_degfx100 = { constraint.i16 },
            convertible_compartment_defrost_termination_temperature_in_degfx100 = { constraint.i16 },
            fresh_food_defrost_heater_max_on_time_in_minutes = { constraint.u8 },
            fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = { constraint.u8 },
            fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = { constraint.u8 },
            convertible_compartment_defrost_heater_max_on_time_in_minutes = { constraint.u8 },
            convertible_compartment_as_fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = { constraint.u8 },
            convertible_compartment_as_freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = { constraint.u8 },
            convertible_compartment_as_fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = { constraint.u8 },
            convertible_compartment_as_freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = { constraint.u8 }
          })
        },
        dwell = {
          constraint.table_keys({
            dwell_time_in_minutes = { constraint.u8 },
            dwell_fresh_food_damper_position = { constraint.in_set(enum.keys(damper_position_type)) },
            dwell_refrigerant_valve_position = { constraint.in_set(enum.keys(sealed_system_valve_position_type)) }
          })
        },
        post_dwell = {
          constraint.table_keys({
            post_dwell_exit_time_in_minutes = { constraint.u8 },
            fresh_food_only_post_dwell_exit_time_in_minutes = { constraint.u8 },
            post_dwell_freezer_evap_exit_temperature_in_degfx100 = { constraint.i16 },
            fresh_food_only_post_dwell_freezer_evap_exit_temperature_in_degfx100 = { constraint.i16 },
            post_dwell_refrigerant_valve_position = { constraint.in_set(enum.keys(sealed_system_valve_position_type)) },
            post_dwell_compressor_speed = { constraint.in_set(enum.keys(compressor_speed_type)) },
            post_dwell_condenser_fan_speed = { constraint.in_set(enum.keys(fan_speed_type)) },
            post_dwell_fresh_food_damper_position = { constraint.in_set(enum.keys(damper_position_type)) }
          })
        }
      }
    )

    if (config.idle.max_time_between_defrosts_in_minutes <= config.idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes) then
      error('max time between defrosts must be greater than minimum time between defrosts abnormal run time', 2)
    end

    if (config.post_dwell.fresh_food_only_post_dwell_exit_time_in_minutes >= config.idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes or
        config.post_dwell.fresh_food_only_post_dwell_exit_time_in_minutes >= config.idle.max_time_between_defrosts_in_minutes) then
      error('fresh food only post dwell time should be less than minimum or maximum time between defrosts', 2)
    end

    if (config.post_dwell.post_dwell_exit_time_in_minutes >= config.idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes or
        config.post_dwell.post_dwell_exit_time_in_minutes >= config.idle.max_time_between_defrosts_in_minutes) then
      error('post dwell time should be less than minimum or maximum time between defrosts', 2)
    end

    return generate(config)
  end
end

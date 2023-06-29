local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local TypedString = require 'lua-common'.util.TypedString
local FillTubeHeater = require '../IceMaker/FillTubeHeater'
local ice_maker_type = require 'IceMaker/IceMakerType'
local ice_maker_location= require 'IceMaker/IceMakerLocation'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)
  local fill_tube_heater = FillTubeHeater(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'twist_tray_ice_maker' },
      structure(
        structure(
          u8(ice_maker_type[config.information.type]),
          u8(ice_maker_location[config.information.location])
        ),
        structure(
          pointer(config.fill.ice_maker_fill_monitor)
        ),
        structure(
          i16(config.freeze.maximum_harvest_temperature_in_deg_fx100),
          i16(config.freeze.start_integration_temperature_in_deg_fx100),
          i16(config.freeze.minimum_freeze_time_initiation_temperature_in_deg_fx100),
          u32(config.freeze.target_integration_temperature_in_deg_fx100_times_seconds),
          u8(config.freeze.minimum_freeze_time_in_minutes)
        ),
        structure(
          u8(config.harvest.full_bucket_wait_period_in_minutes),
          u8(config.harvest.full_bucket_detection_period_in_secx10),
          u8(config.harvest.initial_homing_twist_period_in_secx10),
          u8(config.harvest.home_landing_delay_period_in_secx10 ),
          u8(config.harvest.motor_error_retry_initialize_in_minutes),
          u8(config.harvest.long_motor_error_timeout_period_in_sec),
          u8(config.harvest.short_motor_error_timeout_period_in_sec),
          u8(config.harvest.delay_to_harvest_after_door_closes_in_sec),
          u8(config.harvest.full_bucket_dispense_check_time_in_seconds)
        ),
        fill_tube_heater({
          freeze_thaw_fill_tube_heater_duty_cycle_percentage = config.fill_tube_heater.freeze_thaw_fill_tube_heater_duty_cycle_percentage,
          freeze_thaw_fill_tube_heater_on_time_in_seconds = config.fill_tube_heater.freeze_thaw_fill_tube_heater_on_time_in_seconds,
          non_harvest_fill_tube_heater_duty_cycle_percentage = config.fill_tube_heater.non_harvest_fill_tube_heater_duty_cycle_percentage
        })
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        information = {
          constraint.table_keys({
            type = { constraint.in_set(enum.keys(ice_maker_type)) },
            location = { constraint.in_set(enum.keys(ice_maker_location)) }
          })
        },
        fill = {
          constraint.table_keys({
            ice_maker_fill_monitor = { constraint.typed_string('ice_maker_fill_monitor') }
          })
        },
        freeze = {
          constraint.table_keys({
            maximum_harvest_temperature_in_deg_fx100 = { constraint.i16 },
            start_integration_temperature_in_deg_fx100 = { constraint.i16 },
            minimum_freeze_time_initiation_temperature_in_deg_fx100 = { constraint.i16 },
            target_integration_temperature_in_deg_fx100_times_seconds = { constraint.u32 },
            minimum_freeze_time_in_minutes = { constraint.u8 }
          })
        },
        harvest = {
          constraint.table_keys({
            full_bucket_wait_period_in_minutes = { constraint.u8 },
            full_bucket_detection_period_in_secx10 = { constraint.u8 },
            initial_homing_twist_period_in_secx10 = { constraint.u8 },
            home_landing_delay_period_in_secx10 = { constraint.u8 },
            motor_error_retry_initialize_in_minutes = { constraint.u8 },
            long_motor_error_timeout_period_in_sec = { constraint.u8 },
            short_motor_error_timeout_period_in_sec = { constraint.u8 },
            delay_to_harvest_after_door_closes_in_sec = { constraint.u8 },
            full_bucket_dispense_check_time_in_seconds = { constraint.u8 }
          })
        },
        fill_tube_heater = {
          constraint.table_keys({
            freeze_thaw_fill_tube_heater_duty_cycle_percentage = { constraint.in_range(0,100) },
            freeze_thaw_fill_tube_heater_on_time_in_seconds = { constraint.u16 },
            non_harvest_fill_tube_heater_duty_cycle_percentage = { constraint.in_range(0,100) }
          })
        }
      }
    )

    return generate(config)
  end
end

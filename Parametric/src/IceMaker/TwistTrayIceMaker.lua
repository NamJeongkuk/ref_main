local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local TypedString = require 'lua-common'.util.TypedString
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'twist_tray_ice_maker' },
      structure(
        structure(
          pointer(config.fill.ice_maker_fill_monitor)
        ),
        structure(
          pointer(config.freeze.harvest_count_calculator),
          i16(config.freeze.maximum_harvest_temperature_in_deg_fx100)
        ),
        structure(
          u8(config.harvest.full_state_wait_period_in_minutes),
          u8(config.harvest.full_state_detection_period_in_secx10),
          u8(config.harvest.initial_homing_twist_period_in_secx10),
          u8(config.harvest.home_landing_delay_period_in_secx10 ),
          u8(config.harvest.motor_error_retry_initialize_in_minutes),
          u8(config.harvest.long_motor_error_timeout_period_in_sec),
          u8(config.harvest.short_motor_error_timeout_period_in_sec),
          u8(config.harvest.delay_to_harvest_after_door_opens_in_minutes),
          u8(config.harvest.full_state_dispense_check_time_in_seconds),
          u16(config.harvest.freeze_thaw_fill_tube_heater_on_time_in_seconds),
          u8(config.harvest.freeze_thaw_fill_tube_heater_duty_cycle_percentage),
          i16(config.harvest.full_state_temperature_to_transition_to_freeze_state_in_degfx100),
          u8(config.harvest.full_state_door_open_check_time_in_minutes)
        )
      )
    )
    end)

  return function(config)
    validate_arguments(
      config,
      {
        fill = {
          constraint.table_keys({
            ice_maker_fill_monitor = { constraint.typed_string('ice_maker_fill_monitor') }
          })
        },
        freeze = {
          constraint.table_keys({
            harvest_count_calculator = { constraint.typed_string('harvest_count_calculator') },
            maximum_harvest_temperature_in_deg_fx100 = { constraint.i16 }
          })
        },
        harvest = {
          constraint.table_keys({
            full_state_wait_period_in_minutes = { constraint.u8 },
            full_state_detection_period_in_secx10 = { constraint.u8 },
            initial_homing_twist_period_in_secx10 = { constraint.u8 },
            home_landing_delay_period_in_secx10 = { constraint.u8 },
            motor_error_retry_initialize_in_minutes = { constraint.u8 },
            long_motor_error_timeout_period_in_sec = { constraint.u8 },
            short_motor_error_timeout_period_in_sec = { constraint.u8 },
            delay_to_harvest_after_door_opens_in_minutes = { constraint.u8 },
            full_state_dispense_check_time_in_seconds = { constraint.u8 },
            freeze_thaw_fill_tube_heater_on_time_in_seconds = { constraint.u16 },
            freeze_thaw_fill_tube_heater_duty_cycle_percentage = { constraint.in_range(0,100) },
            full_state_temperature_to_transition_to_freeze_state_in_degfx100 = { constraint.i16 },
            full_state_door_open_check_time_in_minutes = { constraint.u8 }
          })
        }
      }
    )

    return generate(config)
  end
end

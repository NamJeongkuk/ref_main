local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local FillTubeHeater = require '../IceMaker/FillTubeHeater'
local ice_maker_type = require 'IceMaker/IceMakerType'
local ice_maker_location= require 'IceMaker/IceMakerLocation'
local enum = require 'lua-common'.utilities.enum
local seconds_per_minute = 60

return function(core)
  import(core)
  local fill_tube_heater = FillTubeHeater(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'aluminum_mold_ice_maker' },
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
          u32(config.freeze.freeze_integration_limit_in_deg_fx100_times_seconds),
          u8(config.freeze.minimum_freeze_time_in_minutes),
          u8(config.freeze.minimum_feeler_arm_extension_time_in_minutes)
        ),
        structure(
          u8(config.harvest.maximum_harvest_time_in_minutes),
          i16(config.harvest.initial_motor_on_temperature_in_deg_fx100),
          u8(config.harvest.initial_minimum_heater_on_time_in_seconds),
          i16(config.harvest.heater_off_temperature_in_deg_fx100),
          i16(config.harvest.heater_on_temperature_in_deg_fx100),
          u8(config.harvest.rake_not_home_test_time_in_seconds),
          u8(config.harvest.feeler_arm_test_time_in_seconds)
        ),
        structure(
          u8(config.harvest_fault.rake_motor_control_time_in_seconds),
          u16(config.harvest_fault.harvest_fault_max_time_in_minutes)
        ),
        structure(
          i16(config.harvest_fix.heater_off_temperature_in_deg_fx100),
          i16(config.harvest_fix.heater_on_temperature_in_deg_fx100),
          u8(config.harvest_fix.motor_off_time_in_seconds),
          u8(config.harvest_fix.motor_on_time_in_seconds),
          u8(config.harvest_fix.maximum_harvest_fix_time_in_minutes)
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
            freeze_integration_limit_in_deg_fx100_times_seconds = { constraint.u32 },
            minimum_freeze_time_in_minutes = { constraint.u8 },
            minimum_feeler_arm_extension_time_in_minutes = { constraint.u8 }
          })
        },
        harvest = {
          constraint.table_keys({
            maximum_harvest_time_in_minutes = { constraint.u8 },
            initial_motor_on_temperature_in_deg_fx100 = { constraint.i16 },
            initial_minimum_heater_on_time_in_seconds = { constraint.u8 },
            heater_off_temperature_in_deg_fx100 = { constraint.i16 },
            heater_on_temperature_in_deg_fx100 = { constraint.i16 },
            rake_not_home_test_time_in_seconds = { constraint.u8 },
            feeler_arm_test_time_in_seconds = { constraint.u8 }
          })
        },
        harvest_fault = {
          constraint.table_keys({
            rake_motor_control_time_in_seconds = { constraint.u8 },
            harvest_fault_max_time_in_minutes = { constraint.u16 }
          })
        },
        harvest_fix = {
          constraint.table_keys({
            heater_off_temperature_in_deg_fx100 = { constraint.i16 },
            heater_on_temperature_in_deg_fx100 = { constraint.i16 },
            motor_off_time_in_seconds = { constraint.u8 },
            motor_on_time_in_seconds = { constraint.u8 },
            maximum_harvest_fix_time_in_minutes = { constraint.u8 }
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

    if ((config.harvest.maximum_harvest_time_in_minutes * seconds_per_minute) < config.fill_tube_heater.freeze_thaw_fill_tube_heater_on_time_in_seconds) then
      error('freeze thaw fill tube heater on time must be less than or equal to maximum harvest time', 2)
    end

    if (config.fill_tube_heater.freeze_thaw_fill_tube_heater_on_time_in_seconds < config.harvest.initial_minimum_heater_on_time_in_seconds) then
      error('minimum heater on time must be less than or equal to freeze thaw fill tube heater on time', 2)
    end

    return generate(config)
  end
end

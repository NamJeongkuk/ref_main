local TwistTrayIceMaker = require 'IceMaker/TwistTrayIceMaker'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local clone = require 'lua-common'.utilities.clone
local TypedString = require 'lua-common'.util.TypedString
local ice_maker_type = require 'IceMaker/IceMakerType'
local ice_maker_location = require 'IceMaker/IceMakerLocation'

describe('TwistTrayIceMaker', function()
  local twist_tray_ice_maker = TwistTrayIceMaker(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.deep_merge({
      information = {
        type = 'cartridge',
        location = 'fridge'
      },
      fill = {
        ice_maker_fill_monitor = TypedString('ice_maker_fill_monitor', 'ice_maker_fill_monitor')
      },
      freeze = {
        maximum_harvest_temperature_in_deg_fx100 = 1900,
        start_integration_temperature_in_deg_fx100 = 3200,
        target_integration_temperature_in_deg_fx100_times_seconds = 2000000,
        minimum_freeze_time_in_minutes = 50
      },
      harvest = {
        full_bucket_wait_period_in_minutes = 7,
        full_bucket_detection_period_in_secx10 = 8,
        initial_homing_twist_period_in_secx10 = 9,
        home_landing_delay_period_in_secx10 = 10,
        motor_error_retry_initialize_in_minutes = 11,
        long_motor_error_timeout_period_in_sec = 12,
        short_motor_error_timeout_period_in_sec = 13,
        delay_to_harvest_after_door_closes_in_sec = 14,
        full_bucket_dispense_check_time_in_seconds = 15
      },
      fill_tube_heater = {
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100,
        freeze_thaw_fill_tube_heater_on_time_in_seconds = 400,
        non_harvest_fill_tube_heater_duty_cycle_percentage = 10
      }
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(twist_tray_ice_maker, generate_config())
  end)

  it('should assert if information.type is not valid', function()
    should_fail_with("information.type=-1 must be in the set { 'aluminum_mold', 'twist', 'cartridge', 'nugget' }", function()
      twist_tray_ice_maker(generate_config({
        information = {
          type = -1
        }
      }))
    end)
  end)

  it('should assert if infromation.location is not valid', function()
    should_fail_with("information.location=-1 must be in the set { 'freezer', 'fridge' }", function()
      twist_tray_ice_maker(generate_config({
        information = {
          location = -1
        }
      }))
    end)
  end)

  it('should assert if fill.ice_maker_fill_monitor is not a string', function()
    should_fail_with('fill.ice_maker_fill_monitor must be a typed string with type ice_maker_fill_monitor, but is a number', function()
      twist_tray_ice_maker(generate_config({
        fill = {
          ice_maker_fill_monitor = 1
       }
      }))
    end)
  end)

  it('should assert if maximum_harvest_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('maximum_harvest_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      twist_tray_ice_maker(generate_config({
        freeze = {
          maximum_harvest_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if start_integration_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('start_integration_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      twist_tray_ice_maker(generate_config({
        freeze = {
          start_integration_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if target_integration_temperature_in_deg_fx100_times_seconds is not in range', function()
    should_fail_with('target_integration_temperature_in_deg_fx100_times_seconds=-1 must be in [0, 4294967295]', function()
      twist_tray_ice_maker(generate_config({
        freeze = {
          target_integration_temperature_in_deg_fx100_times_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if minimum_freeze_time_in_minutes is not in range', function()
    should_fail_with('minimum_freeze_time_in_minutes=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        freeze = {
          minimum_freeze_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if full_bucket_wait_period_in_minutes is not in range', function()
    should_fail_with('full_bucket_wait_period_in_minutes=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          full_bucket_wait_period_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if full_bucket_detection_period_in_secx10 is not in range', function()
    should_fail_with('full_bucket_detection_period_in_secx10=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          full_bucket_detection_period_in_secx10 = -1
        }
      }))
    end)
  end)

  it('should assert if initial_homing_twist_period_in_secx10 is not in range', function()
    should_fail_with('initial_homing_twist_period_in_secx10=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          initial_homing_twist_period_in_secx10 = -1
        }
      }))
    end)
  end)

  it('should assert if home_landing_delay_period_in_secx10 is not in range', function()
    should_fail_with('home_landing_delay_period_in_secx10=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          home_landing_delay_period_in_secx10 = -1
        }
      }))
    end)
  end)

  it('should assert if motor_error_retry_initialize_in_minutes is not in range', function()
    should_fail_with('motor_error_retry_initialize_in_minutes=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          motor_error_retry_initialize_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if long_motor_error_timeout_period_in_sec is not in range', function()
    should_fail_with('long_motor_error_timeout_period_in_sec=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          long_motor_error_timeout_period_in_sec = -1
        }
      }))
    end)
  end)

  it('should assert if short_motor_error_timeout_period_in_sec is not in range', function()
    should_fail_with('short_motor_error_timeout_period_in_sec=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          short_motor_error_timeout_period_in_sec = -1
        }
      }))
    end)
  end)

  it('should assert if delay_to_harvest_after_door_closes_in_sec is not in range', function()
    should_fail_with('delay_to_harvest_after_door_closes_in_sec=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          delay_to_harvest_after_door_closes_in_sec = -1
        }
      }))
    end)
  end)

  it('should assert if full_bucket_dispense_check_time_in_seconds is not in range', function()
    should_fail_with('full_bucket_dispense_check_time_in_seconds=-1 must be in [0, 255]', function()
      twist_tray_ice_maker(generate_config({
        harvest = {
          full_bucket_dispense_check_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if freeze_thaw_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('freeze_thaw_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      twist_tray_ice_maker(generate_config({
        fill_tube_heater = {
          freeze_thaw_fill_tube_heater_duty_cycle_percentage = 101
        }
      }))
    end)
  end)

  it('should assert if freeze_thaw_fill_tube_heater_on_time_in_seconds is not in range', function()
    should_fail_with('fill_tube_heater.freeze_thaw_fill_tube_heater_on_time_in_seconds=-1 must be in [0, 65535]', function()
      twist_tray_ice_maker(generate_config({
        fill_tube_heater = {
          freeze_thaw_fill_tube_heater_on_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if non_harvest_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('non_harvest_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      twist_tray_ice_maker(generate_config({
        fill_tube_heater = {
          non_harvest_fill_tube_heater_duty_cycle_percentage = 101
        }
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for twist tray ice maker', function()
    local expected = remove_whitespace([[
        structure(
         structure(
            u8(2),
            u8(1)
          ),
          structure(
            pointer(ice_maker_fill_monitor)
          ),
          structure(
            i16(1900),
            i16(3200),
            u32(2000000),
            u8(50)
          ),
          structure(
            u8(7),
            u8(8),
            u8(9),
            u8(10),
            u8(11),
            u8(12),
            u8(13),
            u8(14),
            u8(15)
          ),
          structure(
            u8(100),
            u16(400),
            u8(10)
          )
        )
      ]])

    local actual = twist_tray_ice_maker({
      information = {
        type = 'cartridge',
        location = 'fridge'
      },
      fill = {
        ice_maker_fill_monitor = TypedString('ice_maker_fill_monitor', 'ice_maker_fill_monitor')
      },
      freeze = {
        maximum_harvest_temperature_in_deg_fx100 = 1900,
        start_integration_temperature_in_deg_fx100 = 3200,
        target_integration_temperature_in_deg_fx100_times_seconds = 2000000,
        minimum_freeze_time_in_minutes = 50
      },
      harvest = {
        full_bucket_wait_period_in_minutes = 7,
        full_bucket_detection_period_in_secx10 = 8,
        initial_homing_twist_period_in_secx10 = 9,
        home_landing_delay_period_in_secx10 = 10,
        motor_error_retry_initialize_in_minutes = 11,
        long_motor_error_timeout_period_in_sec = 12,
        short_motor_error_timeout_period_in_sec = 13,
        delay_to_harvest_after_door_closes_in_sec = 14,
        full_bucket_dispense_check_time_in_seconds = 15
      },
      fill_tube_heater = {
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100,
        freeze_thaw_fill_tube_heater_on_time_in_seconds = 400,
        non_harvest_fill_tube_heater_duty_cycle_percentage = 10
      }
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('twist_tray_ice_maker'))
  end)

  it('should memoize', function()
    should_memoize_calls(twist_tray_ice_maker, generate_config())
  end)
end)

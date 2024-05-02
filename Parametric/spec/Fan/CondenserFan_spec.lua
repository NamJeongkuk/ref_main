local CondenserFan = require 'Fan/CondenserFan'
local fan_id = require 'Fan/FanId'
local FanSpeedTable = require 'Fan/FanSpeedTable'
local FanCareAboutCoolingModeSpeedTable = require 'Fan/FanCareAboutCoolingModeSpeedTable'
local Pid = require 'Fan/Pid'
local FanFault = require 'Fan/FanFault'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local DutyCycle = require 'Fan/DutyCycle'
local Rpm = require 'Fan/Rpm'

describe('CondenserFan', function()
  local condenser_fan = CondenserFan(core_mock)
  local fan_speed_table = FanSpeedTable(core_mock)
  local fan_care_about_cooling_mode_speed_table = FanCareAboutCoolingModeSpeedTable(core_mock)
  local pid = Pid(core_mock)
  local fan_fault = FanFault(core_mock)
  local duty_cycle = DutyCycle(core_mock)
  local rpm = Rpm(core_mock)

  local some_valve_speed_table = fan_speed_table({
    super_low_speed = duty_cycle({duty_cycle = 10}),
    low_speed = rpm({rpm = 1000}),
    medium_speed = duty_cycle({duty_cycle = 50}),
    high_speed = rpm({rpm = 1800}),
    super_high_speed = duty_cycle({duty_cycle = 100})
  })

  local some_valve_care_about_coooling_mode_speed_table = fan_care_about_cooling_mode_speed_table({
    care_about_freezer_setpoint = false,
    super_low_speed = duty_cycle({duty_cycle = 10}),
    low_speed_fresh_food = rpm({rpm = 1000}),
    low_speed_freezer = rpm({rpm = 1000}),
    medium_speed_fresh_food = duty_cycle({duty_cycle = 15}),
    medium_speed_freezer = rpm({rpm = 1400}),
    high_speed_fresh_food = duty_cycle({duty_cycle = 16}),
    high_speed_freezer = rpm({rpm = 1600}),
    super_high_speed = duty_cycle({duty_cycle = 18})
  })

  local some_pid = pid({
    kp_value = 11,
    kp_bits = 12,
    ki_value = 13,
    ki_bits = 14,
    kd_value = 15,
    kd_bits = 16,
    lower_limit = 17,
    upper_limit = 18
  })

  local some_fault = fan_fault({
    cannot_reach_target_fault_timeout_in_minutes = 11,
    missing_feedback_when_fan_on_fault_timeout_in_minutes = 12,
    feedback_present_when_fan_off_timeout_in_minutes = 13
  })

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fan_id = fan_id.freezer_evap,
      pulses_per_revolution = 2,
      care_about_cooling_mode = false,
      power_usage_in_watts = 8,
      pid = some_pid,
      fault = some_fault,
      speed_table = TypedString('fan_speed_table', 'fan_speed_table'),
      high_ambient_humidity_and_high_temperature = rpm({rpm = 1000}),
      high_ambient_humidity_and_low_temperature = rpm({rpm = 1200}),
      high_ambient_humidity_and_high_temperature_ff_cooling_mode = rpm({rpm = 800}),
      high_ambient_humidity_and_low_temperature_ff_cooling_mode = rpm({rpm = 900}),
      high_ambient_trigger_temperature_in_degfx100 = 8200,
      high_ambient_trigger_humidity_in_percentx100 = 7600
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(condenser_fan, generate_config())
  end)

  it('should assert if fan_id is not a fan_id', function()
    should_fail_with('fan_id=totally_not_a_fan is not in the fan_id enumeration, allowable values: condenser, convertible_compartment, deli_fan, freezer_evap, fresh_food_evap, ice_cabinet', function()
      condenser_fan(generate_config({
        fan_id = 'totally_not_a_fan'
      }))
    end)
  end)

  it('should assert if pulses_per_revolution is not a u8', function()
    should_fail_with('pulses_per_revolution=50000 must be in [0, 255]', function()
      condenser_fan(generate_config({
        pulses_per_revolution = 50000
      }))
    end)
  end)

  it('should assert if speed_table is not a fan_speed_table when care_about_cooling_mode flag is false', function()
    should_fail_with('speed_table must be a typed string with type fan_speed_table, has type(s) pid', function()
      condenser_fan(generate_config({
        care_about_cooling_mode = false,
        speed_table = TypedString('pid', 'some_pid')
      }))
    end)
  end)

  it('should assert if speed_table is not a fan_care_about_cooling_mode_speed_table when care_about_cooling_mode flag is true', function()
    should_fail_with('speed_table must be a typed string with type fan_care_about_cooling_mode_speed_table, has type(s) pid', function()
      condenser_fan(generate_config({
        care_about_cooling_mode = true,
        speed_table = TypedString('pid', 'some_pid')
      }))
    end)
  end)

  it('should assert if pid is not a pid', function()
    should_fail_with('pid must be a typed string with type pid, has type(s) speed_table', function()
      condenser_fan(generate_config({
        pid = TypedString('speed_table', 'some_speed_table')
      }))
    end)
  end)

  it('should assert if care_about_cooling_mode is not a boolean', function()
    should_fail_with('care_about_cooling_mode must be of type boolean but is of type number', function()
      condenser_fan(generate_config({
        care_about_cooling_mode = 1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('speed_table must be a typed string with type fan_speed_table, but is a number', function()
      condenser_fan(generate_config({
        speed_table = -1
      }))
    end)
  end)

  it('should assert if fields are not a speed', function()
    should_fail_with('high_ambient_humidity_and_high_temperature must be a typed string with type speed, has type(s) blah', function()
      condenser_fan(generate_config({
        high_ambient_humidity_and_high_temperature = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_ambient_humidity_and_low_temperature must be a typed string with type speed, has type(s) blah', function()
      condenser_fan(generate_config({
        high_ambient_humidity_and_low_temperature = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_ambient_humidity_and_high_temperature_ff_cooling_mode must be a typed string with type speed, has type(s) blah', function()
      condenser_fan(generate_config({
        high_ambient_humidity_and_high_temperature_ff_cooling_mode = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_ambient_humidity_and_low_temperature_ff_cooling_mode must be a typed string with type speed, has type(s) blah', function()
      condenser_fan(generate_config({
        high_ambient_humidity_and_low_temperature_ff_cooling_mode = TypedString('blah', 'some_blah')
      }))
    end)
  end)

  it('should assert if high_ambient_trigger_temperature_in_degfx100 is not in range', function()
    should_fail_with('high_ambient_trigger_temperature_in_degfx100=-32769 must be in [-32768, 32767]', function()
      condenser_fan(generate_config({
        high_ambient_trigger_temperature_in_degfx100 = -32769
      }))
    end)
  end)

  it('should assert if high_ambient_trigger_humidity_in_percentx100 is not in range', function()
    should_fail_with('high_ambient_trigger_humidity_in_percentx100=-1 must be in [0, 65535]', function()
      condenser_fan(generate_config({
        high_ambient_trigger_humidity_in_percentx100 = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data with valve rpm table and type condenser_fan', function()
    local expected = remove_whitespace([[
      structure(
        u8(4),
        u8(2),
        bool(false),
        u16(8),
        structure(
          structure(
            i32(11),
            u8(12)
          ),
          structure(
            i32(13),
            u8(14)
          ),
          structure(
            i32(15),
            u8(16)
          ),
          i32(17),
          i32(18)
          ),
          structure(
            u8(11),
            u8(12),
            u8(13)
          ),
        pointer(fan_speed_table),
        structure(
          u8(1),
          u16(1000)
        ),
        structure(
          u8(1),
          u16(1200)
        ),
        structure(
          u8(1),
          u16(900)
        ),
        structure(
          u8(1),
          u16(1100)
        ),
        i16(8200),
        u16(7600)
        )
      ]])

    local actual = condenser_fan({
      fan_id = fan_id.freezer_evap,
      pulses_per_revolution = 2,
      care_about_cooling_mode = false,
      power_usage_in_watts = 8,
      pid = some_pid,
      fault = some_fault,
      speed_table = TypedString('fan_speed_table', 'fan_speed_table'),
      high_ambient_humidity_and_high_temperature = rpm({rpm = 1000}),
      high_ambient_humidity_and_low_temperature = rpm({rpm = 1200}),
      high_ambient_humidity_and_high_temperature_ff_cooling_mode = rpm({rpm = 900}),
      high_ambient_humidity_and_low_temperature_ff_cooling_mode = rpm({rpm = 1100}),
      high_ambient_trigger_temperature_in_degfx100 = 8200,
      high_ambient_trigger_humidity_in_percentx100 = 7600
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('condenser_fan'))
  end)

  it('should generate a typed string with the correct data with valve care about cooling mode rpm table and type condenser_fan', function()
    local expected = remove_whitespace([[
      structure(
        u8(4),
        u8(2),
        bool(true),
        u16(8),
        structure(
          structure(
            i32(11),
            u8(12)
          ),
          structure(
            i32(13),
            u8(14)
          ),
          structure(
            i32(15),
            u8(16)
          ),
          i32(17),
          i32(18)
          ),
          structure(
            u8(11),
            u8(12),
            u8(13)
          ),
        pointer(fan_care_about_cooling_mode_speed_table),
        structure(
          u8(1),
          u16(1000)
        ),
        structure(
          u8(1),
          u16(1200)
        ),
        structure(
          u8(1),
          u16(900)
        ),
        structure(
          u8(1),
          u16(1100)
        ),
        i16(8200),
        u16(7600)
      )
      ]])

    local actual = condenser_fan({
      fan_id = fan_id.freezer_evap,
      pulses_per_revolution = 2,
      care_about_cooling_mode = true,
      power_usage_in_watts = 8,
      pid = some_pid,
      fault = some_fault,
      speed_table = TypedString('fan_care_about_cooling_mode_speed_table', 'fan_care_about_cooling_mode_speed_table'),
      high_ambient_humidity_and_high_temperature = rpm({rpm = 1000}),
      high_ambient_humidity_and_low_temperature = rpm({rpm = 1200}),
      high_ambient_humidity_and_high_temperature_ff_cooling_mode = rpm({rpm = 900}),
      high_ambient_humidity_and_low_temperature_ff_cooling_mode = rpm({rpm = 1100}),
      high_ambient_trigger_temperature_in_degfx100 = 8200,
      high_ambient_trigger_humidity_in_percentx100 = 7600
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('condenser_fan'))
  end)

  it('should memoize', function()
    should_memoize_calls(condenser_fan, generate_config())
  end)

end)

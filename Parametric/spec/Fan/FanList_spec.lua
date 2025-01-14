local FanList = require 'Fan/FanList'
local Fan = require 'Fan/Fan'
local CondenserFan = require 'Fan/CondenserFan'
local fan_id = require 'Fan/FanId'
local SpeedTable = require 'Fan/FanSpeedTable'
local Pid = require 'Fan/Pid'
local FanFault = require 'Fan/FanFault'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local DutyCycle = require 'Fan/DutyCycle'
local TypedString = require 'lua-common'.utilities.TypedString
local Rpm = require 'Fan/Rpm'

describe('FanList', function()
  local fan_list = FanList(core_mock)
  local fan = Fan(core_mock)
  local condenser_fan = CondenserFan(core_mock)
  local speed_table = SpeedTable(core_mock)
  local pid = Pid(core_mock)
  local fan_fault = FanFault(core_mock)
  local duty_cycle = DutyCycle(core_mock)
  local rpm = Rpm(core_mock)

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

  local another_pid = pid({
    kp_value = 20,
    kp_bits = 21,
    ki_value = 22,
    ki_bits = 23,
    kd_value = 24,
    kd_bits = 25,
    lower_limit = 26,
    upper_limit = 27
  })

  local another_fault = fan_fault({
    cannot_reach_target_fault_timeout_in_minutes = 20,
    missing_feedback_when_fan_on_fault_timeout_in_minutes = 21,
    feedback_present_when_fan_off_timeout_in_minutes = 22
  })

  local some_condenser_fan = condenser_fan({
    fan_id = fan_id.condenser,
    pulses_per_revolution = 2,
    care_about_cooling_mode = false,
    power_usage_in_watts = 8,
    pid = some_pid,
    fault = some_fault,
    speed_table = TypedString('fan_speed_table', 'some_speed_table'),
    high_ambient_humidity_and_high_temperature = rpm({rpm = 1000}),
    high_ambient_humidity_and_low_temperature = rpm({rpm = 1025}),
    high_ambient_humidity_and_high_temperature_ff_cooling_mode = rpm({rpm = 900}),
    high_ambient_humidity_and_low_temperature_ff_cooling_mode = rpm({rpm = 925}),
    high_ambient_trigger_temperature_in_degfx100 = 8200,
    high_ambient_trigger_humidity_in_percentx100 = 7500
  })

  local some_fan = fan({
    fan_id = fan_id.freezer_evap,
    pulses_per_revolution = 2,
    care_about_cooling_mode = false,
    power_usage_in_watts = 8,
    pid = some_pid,
    fault = some_fault,
    speed_table = TypedString('fan_speed_table', 'some_speed_table'),
  })

  local another_fan = fan({
    fan_id = fan_id.freezer_evap,
    pulses_per_revolution = 4,
    care_about_cooling_mode = false,
    power_usage_in_watts = 16,
    pid = another_pid,
    fault = another_fault,
    speed_table = TypedString('fan_speed_table', 'some_speed_table'),
  })

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      condenser_fan = some_condenser_fan,
      convertible_compartment_fan = another_fan,
      ice_cabinet_fan = another_fan,
      deli_fan = another_fan,
      freezer_evap_fan = another_fan,
      fresh_food_evap_fan = another_fan,
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(fan_list, generate_config())
  end)

  it('should assert if fans is not of type string fan', function()
    should_fail_with('condenser_fan must be a typed string with type condenser_fan, but is a number', function()
      fan_list(generate_config({
        condenser_fan = 2
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type fan_list', function()
    local expected = remove_whitespace([[
      structure(
        structure(
          u8(0),
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
          pointer(some_speed_table),
          structure(
            u8(1),
            u16(1000)
          ),
          structure(
            u8(1),
            u16(1025)
          ),
          structure(
            u8(1),
            u16(900)
          ),
          structure(
            u8(1),
            u16(925)
          ),
          i16(8200),
          u16(7500)
        ),
        structure(
          u8(4),
          u8(4),
          bool(false),
          u16(16),
          structure(
            structure(
              i32(20),
              u8(21)
            ),
            structure(
              i32(22),
              u8(23)
            ),
            structure(
              i32(24),
              u8(25)
            ),
            i32(26),
            i32(27)
          ),
          structure(
            u8(20),
            u8(21),
            u8(22)
          ),
          pointer(some_speed_table),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          i16(0),
          u16(0)
        ),
        structure(
          u8(4),
          u8(4),
          bool(false),
          u16(16),
          structure(
            structure(
              i32(20),
              u8(21)
            ),
            structure(
              i32(22),
              u8(23)
            ),
            structure(
              i32(24),
              u8(25)
            ),
            i32(26),
            i32(27)
          ),
          structure(
            u8(20),
            u8(21),
            u8(22)
          ),
          pointer(some_speed_table),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          i16(0),
          u16(0)
        ),
        structure(
          u8(4),
          u8(4),
          bool(false),
          u16(16),
          structure(
            structure(
              i32(20),
              u8(21)
            ),
            structure(
              i32(22),
              u8(23)
            ),
            structure(
              i32(24),
              u8(25)
            ),
            i32(26),
            i32(27)
          ),
          structure(
            u8(20),
            u8(21),
            u8(22)
          ),
          pointer(some_speed_table),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          i16(0),
          u16(0)
        ),
        structure(
          u8(4),
          u8(4),
          bool(false),
          u16(16),
          structure(
            structure(
              i32(20),
              u8(21)
            ),
            structure(
              i32(22),
              u8(23)
            ),
            structure(
              i32(24),
              u8(25)
            ),
            i32(26),
            i32(27)
          ),
          structure(
            u8(20),
            u8(21),
            u8(22)
          ),
          pointer(some_speed_table),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          i16(0),
          u16(0)
        ),
        structure(
          u8(4),
          u8(4),
          bool(false),
          u16(16),
          structure(
            structure(
              i32(20),
              u8(21)
            ),
            structure(
              i32(22),
              u8(23)
            ),
            structure(
              i32(24),
              u8(25)
            ),
            i32(26),
            i32(27)
          ),
          structure(
            u8(20),
            u8(21),
            u8(22)
          ),
          pointer(some_speed_table),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          structure(
            u8(1),
            u16(0)
          ),
          i16(0),
          u16(0)
        )
      )
    ]])

    local actual = fan_list({
      condenser_fan = some_condenser_fan,
      convertible_compartment_fan = another_fan,
      ice_cabinet_fan = another_fan,
      deli_fan = another_fan,
      freezer_evap_fan = another_fan,
      fresh_food_evap_fan = another_fan
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fan_list'))
  end)

  it('should memoize', function()
    should_memoize_calls(fan_list, generate_config())
  end)

end)

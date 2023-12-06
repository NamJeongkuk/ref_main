local CabinetTemperatureExceeded = require 'Cooling/CabinetTemperatureExceeded'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('CabinetTemperatureExceeded', function()
  local cabinet_temperature_exceeded = CabinetTemperatureExceeded(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 = 2000,
      temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 = 1500,
      minimum_time_since_power_on_in_minutes = 8 * 60,
      minimum_time_doors_must_be_closed_in_minutes = 60,
      minimum_time_since_defrost_has_completed_in_minutes = 60
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(cabinet_temperature_exceeded, generate_config())
  end)

  it('should assert if temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 is not in range', function()
    should_fail_with('temperature_upper_hysteresis_limit_above_setpoint_in_degfx100=32768 must be in [-32768, 32767]', function()
      cabinet_temperature_exceeded(generate_config({
        temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 = 32768,
        temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 = 0
      }))
    end)
  end)

  it('should assert if temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 is not in range', function()
    should_fail_with('temperature_lower_hysteresis_limit_above_setpoint_in_degfx100=-32770 must be in [-32768, 32767]', function()
      cabinet_temperature_exceeded(generate_config({
        temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 = -32768,
        temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 = -32770
      }))
    end)
  end)

  it('should assert if temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 is less than or equal to temperature_lower_hysteresis_limit_above_setpoint_in_degfx100', function()
    should_fail_with('temperature_lower_hysteresis_limit_above_setpoint_in_degfx100=31 must be <30', function()
      cabinet_temperature_exceeded(generate_config({
        temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 = 30,
        temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 = 31
      }))
    end)
  end)

  it('should assert if temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 is less than or equal to temperature_lower_hysteresis_limit_above_setpoint_in_degfx100', function()
    should_fail_with('temperature_lower_hysteresis_limit_above_setpoint_in_degfx100=30 must be <30', function()
      cabinet_temperature_exceeded(generate_config({
        temperature_upper_hysteresis_limit_above_setpoint_in_degfx100 = 30,
        temperature_lower_hysteresis_limit_above_setpoint_in_degfx100 = 30
      }))
    end)
  end)

  it('should assert if minimum_time_since_power_on_in_minutes is not in range', function()
    should_fail_with('minimum_time_since_power_on_in_minutes=-1 must be in [0, 65535]', function()
      cabinet_temperature_exceeded(generate_config({
        minimum_time_since_power_on_in_minutes = -1
      }))
    end)
  end)

  it('should assert if minimum_time_doors_must_be_closed_in_minutes is not in range', function()
    should_fail_with('minimum_time_doors_must_be_closed_in_minutes=-1 must be in [0, 255]', function()
      cabinet_temperature_exceeded(generate_config({
        minimum_time_doors_must_be_closed_in_minutes = -1
      }))
    end)
  end)

  it('should assert if minimum_time_since_defrost_has_completed_in_minutes is not in range', function()
    should_fail_with('minimum_time_since_defrost_has_completed_in_minutes=-1 must be in [0, 255]', function()
      cabinet_temperature_exceeded(generate_config({
        minimum_time_since_defrost_has_completed_in_minutes = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type cabinet_temperature_exceeded', function()
    local expected = remove_whitespace([[
      structure(
        i16(2000),
        i16(1500),
        u16(480),
        u8(60),
        u8(60)
      )
    ]])

    local actual = cabinet_temperature_exceeded(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('cabinet_temperature_exceeded'))
  end)

  it('should memoize', function()
    should_memoize_calls(cabinet_temperature_exceeded, generate_config())
  end)

end)

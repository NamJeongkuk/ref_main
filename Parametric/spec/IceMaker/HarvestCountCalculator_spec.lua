local HarvestCountCalculator = require 'IceMaker/HarvestCountCalculator'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('HarvestCountCalculator', function()
  local harvest_count_calculator = HarvestCountCalculator(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      start_integration_temperature_in_deg_fx100 = 3200,
      minimum_freeze_time_initiation_temperature_in_deg_fx100 = 3200,
      target_integration_limit_in_deg_fx100_times_seconds = 2000000,
      minimum_freeze_time_in_minutes = 50
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(harvest_count_calculator, generate_config())
  end)

  it('should assert if start_integration_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('start_integration_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      harvest_count_calculator(generate_config({
        start_integration_temperature_in_deg_fx100 = 32768
      }))
    end)
  end)

  it('should assert if minimum_freeze_time_initiation_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('minimum_freeze_time_initiation_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      harvest_count_calculator(generate_config({
        minimum_freeze_time_initiation_temperature_in_deg_fx100 = 32768
      }))
    end)
  end)

  it('should assert if target_integration_limit_in_deg_fx100_times_seconds is not in range', function()
    should_fail_with('target_integration_limit_in_deg_fx100_times_seconds=-1 must be in [0, 4294967295]', function()
      harvest_count_calculator(generate_config({
        target_integration_limit_in_deg_fx100_times_seconds = -1
      }))
    end)
  end)

  it('should assert if minimum_freeze_time_in_minutes is not in range', function()
    should_fail_with('minimum_freeze_time_in_minutes=-1 must be in [0, 255]', function()
      harvest_count_calculator(generate_config({
        minimum_freeze_time_in_minutes = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for harvest count calculator', function()
    local expected = remove_whitespace([[
      structure(
        i16(3200),
        i16(3200),
        u32(2000000),
        u8(50)
      )
    ]])

    local actual = harvest_count_calculator({
      start_integration_temperature_in_deg_fx100 = 3200,
      minimum_freeze_time_initiation_temperature_in_deg_fx100 = 3200,
      target_integration_limit_in_deg_fx100_times_seconds = 2000000,
      minimum_freeze_time_in_minutes = 50
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('harvest_count_calculator'))
  end)

  it('should memoize', function()
    should_memoize_calls(harvest_count_calculator, generate_config())
  end)
end)

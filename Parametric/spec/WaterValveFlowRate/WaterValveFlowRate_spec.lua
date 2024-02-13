local WaterValveFlowRate = require 'WaterValveFlowRate/WaterValveFlowRate'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('WaterValveFlowRate', function()
  local water_valve_flow_rate = WaterValveFlowRate(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      icemaker0_valve_flow_rate_ounces_per_sec_x100 = 0,
      icemaker1_valve_flow_rate_ounces_per_sec_x100 = 0,
      icemaker2_valve_flow_rate_ounces_per_sec_x100 = 0,
      dispenser_valve_flow_rate_ounces_per_sec_x100 = 0
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(water_valve_flow_rate, generate_config())
  end)

  it('should assert if icemaker0_valve_flow_rate_ounces_per_sec_x100 is not in range', function()
    should_fail_with('icemaker0_valve_flow_rate_ounces_per_sec_x100=-1 must be in [0, 255]', function()
      water_valve_flow_rate(generate_config({
        icemaker0_valve_flow_rate_ounces_per_sec_x100 = -1
      }))
    end)
  end)

  it('should assert if icemaker1_valve_flow_rate_ounces_per_sec_x100 is not in range', function()
    should_fail_with('icemaker1_valve_flow_rate_ounces_per_sec_x100=-1 must be in [0, 255]', function()
      water_valve_flow_rate(generate_config({
        icemaker1_valve_flow_rate_ounces_per_sec_x100 = -1
      }))
    end)
  end)

  it('should assert if icemaker2_valve_flow_rate_ounces_per_sec_x100 is not in range', function()
    should_fail_with('icemaker2_valve_flow_rate_ounces_per_sec_x100=-1 must be in [0, 255]', function()
      water_valve_flow_rate(generate_config({
        icemaker2_valve_flow_rate_ounces_per_sec_x100 = -1
      }))
    end)
  end)

  it('should assert if dispenser_valve_flow_rate_ounces_per_sec_x100 is not in range', function()
    should_fail_with('dispenser_valve_flow_rate_ounces_per_sec_x100=-1 must be in [0, 255]', function()
      water_valve_flow_rate(generate_config({
        dispenser_valve_flow_rate_ounces_per_sec_x100 = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type water_valve_flow_rate', function()
    local expected = remove_whitespace([[
      structure(
        u8(0),
        u8(0),
        u8(0),
        u8(0)
      )
      ]])

    local actual = water_valve_flow_rate({
      icemaker0_valve_flow_rate_ounces_per_sec_x100 = 0,
      icemaker1_valve_flow_rate_ounces_per_sec_x100 = 0,
      icemaker2_valve_flow_rate_ounces_per_sec_x100 = 0,
      dispenser_valve_flow_rate_ounces_per_sec_x100 = 0
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('water_valve_flow_rate'))
  end)

  it('should memoize', function()
    should_memoize_calls(water_valve_flow_rate, generate_config())
  end)
end)

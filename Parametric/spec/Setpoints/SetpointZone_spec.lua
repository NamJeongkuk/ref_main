local SetpointZone = require 'Setpoints/SetpointZone'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('SetpointZone', function()
  local setpoint_zone = SetpointZone(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      freezer_trigger_cold_setpoint_zone_in_degfx100 = 200,
      freezer_trigger_warm_setpoint_zone_in_degfx100 = 300,
      fresh_food_trigger_cold_setpoint_zone_in_degfx100 = 400,
      fresh_food_trigger_warm_setpoint_zone_in_degfx100 = 500,
      convertible_compartment_trigger_setpoint_zone_in_degfx100 = 600
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(setpoint_zone, generate_config())
  end)

  it('should assert if freezer_trigger_cold_setpoint_zone_in_degfx100 is not in range', function()
    should_fail_with('freezer_trigger_cold_setpoint_zone_in_degfx100=-100000 must be in [-32768, 32767]', function()
      setpoint_zone(generate_config({
        freezer_trigger_cold_setpoint_zone_in_degfx100= -100000
      }))
    end)
  end)

  it('should assert if freezer_trigger_warm_setpoint_zone_in_degfx100 is not in range', function()
    should_fail_with('freezer_trigger_warm_setpoint_zone_in_degfx100=-100000 must be in [-32768, 32767]', function()
      setpoint_zone(generate_config({
        freezer_trigger_warm_setpoint_zone_in_degfx100= -100000
      }))
    end)
  end)

  it('should assert if fresh_food_trigger_cold_setpoint_zone_in_degfx100 is not in range', function()
    should_fail_with('fresh_food_trigger_cold_setpoint_zone_in_degfx100=-100000 must be in [-32768, 32767]', function()
      setpoint_zone(generate_config({
         fresh_food_trigger_cold_setpoint_zone_in_degfx100= -100000
      }))
    end)
  end)

  it('should assert if fresh_food_trigger_warm_setpoint_zone_in_degfx100 is not in range', function()
    should_fail_with('fresh_food_trigger_warm_setpoint_zone_in_degfx100=-100000 must be in [-32768, 32767]', function()
      setpoint_zone(generate_config({
         fresh_food_trigger_warm_setpoint_zone_in_degfx100= -100000
      }))
    end)
  end)

  it('should assert if convertible_compartment_trigger_setpoint_zone_in_degfx100 is not in range', function()
    should_fail_with('convertible_compartment_trigger_setpoint_zone_in_degfx100=-100000 must be in [-32768, 32767]', function()
      setpoint_zone(generate_config({
         convertible_compartment_trigger_setpoint_zone_in_degfx100= -100000
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type setpoint_zone', function()
    local expected = remove_whitespace([[
    structure(
      i16(200),
      i16(300),
      i16(400),
      i16(500),
      i16(600))
    ]])

    local actual = setpoint_zone({
      freezer_trigger_cold_setpoint_zone_in_degfx100 = 200,
      freezer_trigger_warm_setpoint_zone_in_degfx100 = 300,
      fresh_food_trigger_cold_setpoint_zone_in_degfx100 = 400,
      fresh_food_trigger_warm_setpoint_zone_in_degfx100 = 500,
      convertible_compartment_trigger_setpoint_zone_in_degfx100 = 600
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('setpoint_zone'))
  end)

  it('should memoize', function()
    should_memoize_calls(setpoint_zone, generate_config())
  end)
end)

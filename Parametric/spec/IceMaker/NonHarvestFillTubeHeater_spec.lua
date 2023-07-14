local NonHarvestFillTubeHeater = require 'IceMaker/NonHarvestFillTubeHeater'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('NonHarvestFillTubeHeater', function()
  local non_harvest_fill_tube_heater = NonHarvestFillTubeHeater(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      maximum_cabinet_temperature_in_deg_fx100 = 2000,
      formation_fill_tube_heater_duty_cycle_percentage = 10,
      maintenance_fill_tube_heater_duty_cycle_percentage = 9,
      off_fill_tube_heater_duty_cycle_percentage = 8
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(non_harvest_fill_tube_heater, generate_config())
  end)

  it('should assert if maximum_cabinet_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('maximum_cabinet_temperature_in_deg_fx100=-100000 must be in [-32768, 32767]', function()
      non_harvest_fill_tube_heater(generate_config({
        maximum_cabinet_temperature_in_deg_fx100 = -100000
      }))
    end)
  end)

  it('should assert if formation_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('formation_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      non_harvest_fill_tube_heater(generate_config({
        formation_fill_tube_heater_duty_cycle_percentage = 101
      }))
    end)
  end)

  it('should assert if maintenance_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('maintenance_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      non_harvest_fill_tube_heater(generate_config({
        maintenance_fill_tube_heater_duty_cycle_percentage = 101
      }))
    end)
  end)

  it('should assert if off_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('off_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      non_harvest_fill_tube_heater(generate_config({
        off_fill_tube_heater_duty_cycle_percentage = 101
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for fill tube heater', function()
    local expected = remove_whitespace([[
        structure(
          i16(2000),
          u8(10),
          u8(9),
          u8(0)
        )
      ]])

    local actual = non_harvest_fill_tube_heater({
      maximum_cabinet_temperature_in_deg_fx100 = 2000,
      formation_fill_tube_heater_duty_cycle_percentage = 10,
      maintenance_fill_tube_heater_duty_cycle_percentage = 9,
      off_fill_tube_heater_duty_cycle_percentage = 0
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('non_harvest_fill_tube_heater'))
  end)

  it('should memoize', function()
    should_memoize_calls(non_harvest_fill_tube_heater, generate_config())
  end)
end)

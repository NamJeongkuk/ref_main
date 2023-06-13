local FillTubeHeater = require 'IceMaker/FillTubeHeater'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('FillTubeHeater', function()
  local fill_tube_heater = FillTubeHeater(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100,
      freeze_thaw_fill_tube_heater_on_time_in_seconds = 420,
      non_harvest_fill_tube_heater_duty_cycle_percentage = 10
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(fill_tube_heater, generate_config())
  end)

  it('should assert if freeze_thaw_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('freeze_thaw_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      fill_tube_heater(generate_config({
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = 101
      }))
    end)
  end)

  it('should assert if freeze_thaw_fill_tube_heater_on_time_in_seconds is not in range', function()
    should_fail_with('freeze_thaw_fill_tube_heater_on_time_in_seconds=-1 must be in [0, 65535]', function()
      fill_tube_heater(generate_config({
        freeze_thaw_fill_tube_heater_on_time_in_seconds = -1
      }))
    end)
  end)

  it('should assert if non_harvest_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('non_harvest_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      fill_tube_heater(generate_config({
        non_harvest_fill_tube_heater_duty_cycle_percentage = 101
      }))
    end)
  end)

  it('should assert if freeze_thaw_fill_tube_heater_duty_cycle_percentage is 0 and freeze_thaw_fill_tube_heater_on_time_in_seconds is not 0', function()
    should_fail_with('if freeze_thaw_fill_tube_heater_duty_cycle_percentage is 0, then freeze_thaw_fill_tube_heater_on_time_in_seconds must be 0', function()
      fill_tube_heater(generate_config({
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = 0,
        freeze_thaw_fill_tube_heater_on_time_in_seconds = 200
      }))
    end)
  end)

  it('should assert if freeze_thaw_fill_tube_heater_on_time_in_seconds is 0 and freeze_thaw_fill_tube_heater_duty_cycle_percentage is not 0', function()
    should_fail_with('if freeze_thaw_fill_tube_heater_on_time_in_seconds is 0, then freeze_thaw_fill_tube_heater_duty_cycle_percentage must be 0', function()
      fill_tube_heater(generate_config({
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = 88,
        freeze_thaw_fill_tube_heater_on_time_in_seconds = 0
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for fill tube heater', function()
    local expected = remove_whitespace([[
        structure(
          u8(100),
          u16(420),
          u8(10)
        )
      ]])

    local actual = fill_tube_heater({
      freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100,
      freeze_thaw_fill_tube_heater_on_time_in_seconds = 420,
      non_harvest_fill_tube_heater_duty_cycle_percentage = 10
    })
    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fill_tube_heater'))
  end)

  it('should memoize', function()
    should_memoize_calls(fill_tube_heater, generate_config())
  end)
end)

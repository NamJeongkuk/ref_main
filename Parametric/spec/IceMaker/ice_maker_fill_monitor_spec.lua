local IceMakerFillMonitor = require 'IceMaker/IceMakerFillMonitor'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('ice_maker_fill_monitor', function()
  local ice_maker_fill_monitor = IceMakerFillMonitor(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      timed_ice_maker_fill_in_secondsx10 = 10,
      precise_icemaker_fill_max_time_in_secondsx10 = 20,
      precise_icemaker_fill_ounces_in_ozx100 = 30
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_maker_fill_monitor, generate_config())
  end)

  it('should assert if precise_icemaker_fill_max_time_in_secondsx10 is a string', function()
    should_fail_with('attempt to compare string with number', function()
      ice_maker_fill_monitor(generate_config({
        precise_icemaker_fill_max_time_in_secondsx10 = "not a number"
      }))
    end)
  end)

  it('should assert if precise_icemaker_fill_ounces_in_ozx100 is a string', function()
    should_fail_with('precise_icemaker_fill_ounces_in_ozx100 must be of type number but is of type string', function()
      ice_maker_fill_monitor(generate_config({
        precise_icemaker_fill_ounces_in_ozx100 = "30"
      }))
    end)
  end)

  it('should assert if precise_icemaker_fill_max_time_in_secondsx10 is less than or equal to timed_ice_maker_fill_in_secondsx10', function()
    should_fail_with('precise_icemaker_fill_max_time_in_secondsx10=11 must be >30', function()
      ice_maker_fill_monitor(generate_config({
        timed_ice_maker_fill_in_secondsx10 = 30,
        precise_icemaker_fill_max_time_in_secondsx10 = 11
      }))
    end)

    should_fail_with('precise_icemaker_fill_max_time_in_secondsx10=50 must be >50', function()
      ice_maker_fill_monitor(generate_config({
        timed_ice_maker_fill_in_secondsx10 = 50,
        precise_icemaker_fill_max_time_in_secondsx10 = 50
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for freezer ice rate', function()
    local expected = remove_whitespace([[
        structure(
          u16(10),
          u16(20),
          u16(30)
        )
      ]])

    local actual = ice_maker_fill_monitor({
      timed_ice_maker_fill_in_secondsx10 = 10,
      precise_icemaker_fill_max_time_in_secondsx10 = 20,
      precise_icemaker_fill_ounces_in_ozx100 = 30
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker_fill_monitor'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_maker_fill_monitor, generate_config())
  end)
end)

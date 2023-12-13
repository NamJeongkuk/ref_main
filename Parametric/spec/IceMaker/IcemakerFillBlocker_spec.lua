local IceMakerFillBlocker = require 'IceMaker/IceMakerFillBlocker'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('IceMakerFillBlocker', function()
  local ice_maker_fill_blocker = IceMakerFillBlocker(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      new_filter_dispenser_purge_volume_before_icemaker_fill_in_ounces_x100 = 0,
      icemaker_new_filter_fill_state_delay_in_minutes = 1440
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_maker_fill_blocker, generate_config())
  end)

  it('should assert if new_filter_dispenser_purge_volume_before_icemaker_fill_in_ounces_x100 is not in range', function()
    should_fail_with('new_filter_dispenser_purge_volume_before_icemaker_fill_in_ounces_x100=-1 must be in [0, 4294967295]', function()
      ice_maker_fill_blocker(generate_config({
         new_filter_dispenser_purge_volume_before_icemaker_fill_in_ounces_x100 = -1
      }))
    end)
  end)

  it('should assert if icemaker_new_filter_fill_state_delay_in_minutes is not in range', function()
    should_fail_with('icemaker_new_filter_fill_state_delay_in_minutes=-1 must be in [0, 65535]', function()
      ice_maker_fill_blocker(generate_config({
         icemaker_new_filter_fill_state_delay_in_minutes = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type ice_maker_fill_blocker', function()
    local expected = remove_whitespace([[
      structure(
        u32(0),
        u16(1440)
      )
      ]])

    local actual = ice_maker_fill_blocker({
      new_filter_dispenser_purge_volume_before_icemaker_fill_in_ounces_x100 = 0,
      icemaker_new_filter_fill_state_delay_in_minutes = 1440
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker_fill_blocker'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_maker_fill_blocker, generate_config())
  end)
end)

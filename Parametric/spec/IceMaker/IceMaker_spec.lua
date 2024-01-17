local IceMaker = require 'IceMaker/IceMaker'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('IceMaker', function()
  local ice_maker = IceMaker(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      ice_maker_slots = TypedString('ice_maker_slots', 'ice_maker_slots'),
      ice_maker_fill_blocker = TypedString('ice_maker_fill_blocker', 'ice_maker_fill_blocker')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(ice_maker, generate_config())
  end)

  it('should assert if ice_maker_slots is not a string', function()
    should_fail_with('ice_maker_slots must be a typed string with type ice_maker_slots, but is a number', function()
      ice_maker(generate_config({
        ice_maker_slots = 1
      }))
    end)
  end)

  it('should assert if ice_maker_fill_blocker is not a string', function()
    should_fail_with('ice_maker_fill_blocker must be a typed string with type ice_maker_fill_blocker, but is a number', function()
      ice_maker(generate_config({
        ice_maker_fill_blocker = 6
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for ice_maker', function()
    local expected = remove_whitespace([[
        structure(
          pointer(ice_maker_slots),
          pointer(ice_maker_fill_blocker)
        )
      ]])

    local actual = ice_maker({
      ice_maker_slots = TypedString('ice_maker_slots', 'ice_maker_slots'),
      ice_maker_fill_blocker = TypedString('ice_maker_fill_blocker', 'ice_maker_fill_blocker')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('ice_maker'))
  end)

  it('should memoize', function()
    should_memoize_calls(ice_maker, generate_config())
  end)
end)

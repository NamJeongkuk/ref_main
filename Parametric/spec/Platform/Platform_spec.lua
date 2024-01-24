local Platform = require 'Platform/Platform'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local platform_type = require 'Platform.PlatformType'

describe('Platform', function()
  local platform = Platform(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      platform_type = 'four_door'
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(platform, generate_config())
  end)

  it('should require platform_type to be a valid platform type', function()
    should_fail_with("platform_type='among_us_refrigerator' must be in the set { 'side_by_side', 'four_door', 'all_freezer', 'all_fresh_food' }", function()
      platform(generate_config({
        platform_type = 'among_us_refrigerator'
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type platform', function()
    local expected = remove_whitespace([[
     structure(
       u8(]] .. platform_type.four_door .. [[)
     )
   ]])

    local actual = platform(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('platform'))
  end)

  it('should memoize', function()
    should_memoize_calls(platform, generate_config())
  end)
end)

local Platform = require 'Platform/Platform'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('Platform', function()
  local platform = Platform(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      compartments = { 'fresh_food', 'freezer', 'convertible' },
      number_of_evaporators = 2
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(platform, generate_config())
  end)

  it('should assert if number_of_evaporators is not in range', function()
    should_fail_with('number_of_evaporators=-1 must be in [0, 255]', function()
      platform(generate_config({
        number_of_evaporators = -1
      }))
    end)
  end)

  it('should require that compartments are in compartment_enum', function()
    should_fail_with("compartments[1]='not a compartment' must be in the set { 'fresh_food', 'freezer', 'convertible' }", function()
      platform(generate_config({
        compartments = { 'not a compartment' }
      }))
    end)

    should_fail_with("compartments[2]='not a compartment' must be in the set { 'fresh_food', 'freezer', 'convertible' }", function()
      platform(generate_config({
        compartments = { 'fresh_food', 'not a compartment' }
      }))
    end)

    should_fail_with("compartments[2]='not a compartment' must be in the set { 'fresh_food', 'freezer', 'convertible' }", function()
      platform(generate_config({
        compartments = { 'fresh_food', 'not a compartment', 'definitely not a compartment' }
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type platform', function()
    local expected = remove_whitespace([[
      structure(
        u8(7),
        u8(2)
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

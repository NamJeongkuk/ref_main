local EnhancedSabbath = require 'EnhancedSabbath/EnhancedSabbath'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('enhancedSabbath', function()
  local enhancedSabbath = EnhancedSabbath(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
        number_of_fresh_food_defrosts_before_freezer_defrost = 3,
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(enhancedSabbath, generate_config())
  end)

  it('should assert if number_of_fresh_food_defrosts_before_freezer_defrost is not in range', function()
    should_fail_with('number_of_fresh_food_defrosts_before_freezer_defrost=-1 must be in [0, 255]', function()
      enhancedSabbath(generate_config({
        number_of_fresh_food_defrosts_before_freezer_defrost = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type enhancedSabbath', function()
    local expected = remove_whitespace([[
      structure(
        u8(3)
      )
    ]])

    local actual = enhancedSabbath(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('enhancedSabbath'))
  end)

  it('should memoize', function()
    should_memoize_calls(enhancedSabbath, generate_config())
  end)

end)

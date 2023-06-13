local Sabbath = require 'Sabbath/Sabbath'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('sabbath', function()
  local sabbath = Sabbath(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      max_time_between_defrosts_in_minutes = 16 * 60
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(sabbath, generate_config())
  end)

  it('should assert if max_time_between_defrosts_in_minutes is not in range', function()
    should_fail_with('max_time_between_defrosts_in_minutes=-1 must be in [0, 65535]', function()
      sabbath(generate_config({
        max_time_between_defrosts_in_minutes = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type sabbath', function()
    local expected = remove_whitespace([[
      structure(
        u16(1440)
      )
      ]])

    local actual = sabbath({
      max_time_between_defrosts_in_minutes = 1440
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('sabbath'))
  end)

  it('should memoize', function()
    should_memoize_calls(sabbath, generate_config())
  end)

end)

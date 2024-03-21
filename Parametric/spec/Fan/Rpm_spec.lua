local Rpm = require 'Fan/Rpm'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('Rpm', function()
  local rpm = Rpm(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      rpm = 2000,
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(rpm, generate_config())
  end)

  it('should assert if rpm is not in range', function()
    should_fail_with('rpm=70000 must be in [0, 65535]', function()
      rpm(generate_config({
        rpm = 70000
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type rpm', function()
    local expected = remove_whitespace([[
      structure(
        u8(1),
        u16(2200))
      ]])

    local actual = rpm({
      rpm = 2200,
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('speed'))
  end)

  it('should memoize', function()
    should_memoize_calls(rpm, generate_config())
  end)

end)

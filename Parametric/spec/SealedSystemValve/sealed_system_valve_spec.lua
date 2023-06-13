local SealedSystemValve = require 'SealedSystemValve/SealedSystemValve'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('sealedSystemValve', function()
  local sealedSystemValve = SealedSystemValve(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      homing_time_in_ms = 200
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(sealedSystemValve, generate_config())
  end)

  it('should assert if homing_time_in_ms is not in range', function()
    should_fail_with('homing_time_in_ms=-1 must be in [0, 65535]', function()
      sealedSystemValve(generate_config({
        homing_time_in_ms = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type sealedSystemValve', function()
    local expected = remove_whitespace([[
    structure(
      u16(200))
    ]])

    local actual = sealedSystemValve({
      homing_time_in_ms = 200
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('sealedSystemValve'))
  end)

  it('should memoize', function()
    should_memoize_calls(sealedSystemValve, generate_config())
  end)

end)

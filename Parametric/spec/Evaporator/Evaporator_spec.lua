local Evaporator = require 'Evaporator/Evaporator'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('Evaporator', function()
  local evaporator = Evaporator(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      number_evaporators = 2
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(evaporator, generate_config())
  end)

  it('should assert if number_evaporators is not in range', function()
    should_fail_with('number_evaporators=-1 must be in [0, 255]', function()
      evaporator(generate_config({
        number_evaporators = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type evaporator', function()
    local expected = remove_whitespace([[
      structure(
      u8(1))
      ]])

    local actual = evaporator({
      number_evaporators = 1
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('evaporator'))
  end)

	it('should memoize', function()
    should_memoize_calls(evaporator, generate_config())
  end)
end)

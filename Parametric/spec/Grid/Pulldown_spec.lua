local Pulldown = require 'Grid/Pulldown'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('Pulldown', function()
  local pulldown = Pulldown(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      pulldown_in_medium_compressor_speed_enable = true,
      compressor_trip_mitigation_enable = true
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(pulldown, generate_config())
  end)

  it('should assert if pulldown_in_medium_compressor_speed_enable is not a boolean', function()
    should_fail_with('pulldown_in_medium_compressor_speed_enable must be of type boolean but is of type number', function()
      pulldown(generate_config({
        pulldown_in_medium_compressor_speed_enable = -1
      }))
    end)
  end)

  it('should assert if compressor_trip_mitigation_enable is not a boolean', function()
    should_fail_with('compressor_trip_mitigation_enable must be of type boolean but is of type number', function()
      pulldown(generate_config({
        compressor_trip_mitigation_enable = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type pulldown', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        bool(true)
      )
    ]])

    local actual = pulldown({
      pulldown_in_medium_compressor_speed_enable = true,
      compressor_trip_mitigation_enable = true
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('pulldown'))
  end)

  it('should memoize', function()
    should_memoize_calls(pulldown, generate_config())
  end)
end)

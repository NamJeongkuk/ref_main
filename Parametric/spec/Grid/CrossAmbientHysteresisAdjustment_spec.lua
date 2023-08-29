local CrossAmbientHysteresisAdjustment = require 'Grid/CrossAmbientHysteresisAdjustment'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('CrossAmbientHysteresisAdjustment', function()
  local cross_ambient_hysteresis_adjustment = CrossAmbientHysteresisAdjustment(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      multiplier = -20,
      divider = 500
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(cross_ambient_hysteresis_adjustment, generate_config())
  end)

  it('should assert if multiplier is not in range', function()
    should_fail_with('multiplier=999999 must be in [-32768, 32767]', function()
      cross_ambient_hysteresis_adjustment(generate_config({
         multiplier = 999999
      }))
    end)
  end)

  it('should assert if divider is not in range', function()
    should_fail_with('divider=999999 must be in [-32768, 32767]', function()
      cross_ambient_hysteresis_adjustment(generate_config({
         divider = 999999
      }))
    end)
  end)

  it('should assert if divider is zero', function()
    should_fail_with('divider=0 must not be equal to 0', function()
      cross_ambient_hysteresis_adjustment(generate_config({
         divider = 0
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type cross_ambient_hysteresis_adjustment', function()
    local expected = remove_whitespace([[
        structure(
          i16(-20),
          i16(500)
        )
    ]])

    local actual = cross_ambient_hysteresis_adjustment({
      multiplier = -20,
      divider = 500
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('cross_ambient_hysteresis_adjustment'))
  end)

  it('should memoize', function()
    should_memoize_calls(cross_ambient_hysteresis_adjustment, generate_config())
  end)
end)

local CoolingModeIndependentCompressorSpeeds = require 'Compressor/CoolingModeIndependentCompressorSpeeds'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('coolingModeIndependentCompressorSpeeds', function()
  local coolingModeIndependentCompressorSpeeds = CoolingModeIndependentCompressorSpeeds(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      low_speed_frequency_in_hz = 40,
      medium_speed_frequency_in_hz = 50,
      high_speed_frequency_in_hz = 60
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(coolingModeIndependentCompressorSpeeds, generate_config())
  end)

  it('should assert if low_speed_frequency_in_hz is not in range', function()
    should_fail_with('low_speed_frequency_in_hz=-1 must be in [0, 255]', function()
      coolingModeIndependentCompressorSpeeds(generate_config({
        low_speed_frequency_in_hz = -1
      }))
    end)
  end)

  it('should assert if medium_speed_frequency_in_hz is not in range', function()
    should_fail_with('medium_speed_frequency_in_hz=-1 must be in [0, 255]', function()
      coolingModeIndependentCompressorSpeeds(generate_config({
        medium_speed_frequency_in_hz = -1
      }))
    end)
  end)

  it('should assert if high_speed_frequency_in_hz is not in range', function()
    should_fail_with('high_speed_frequency_in_hz=-1 must be in [0, 255]', function()
      coolingModeIndependentCompressorSpeeds(generate_config({
        high_speed_frequency_in_hz = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for independent compressor speeds', function()
    local expected = remove_whitespace([[
      structure(
        u8(40),
        u8(50),
        u8(60)
      )
    ]])

    local actual = coolingModeIndependentCompressorSpeeds({
      low_speed_frequency_in_hz = 40,
      medium_speed_frequency_in_hz = 50,
      high_speed_frequency_in_hz = 60
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('cooling_mode_independent_compressor_speeds'))
  end)

  it('should memoize', function()
    should_memoize_calls(coolingModeIndependentCompressorSpeeds, generate_config())
  end)
end)

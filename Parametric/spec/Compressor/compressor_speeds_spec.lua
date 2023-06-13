local CompressorSpeeds = require 'Compressor/CompressorSpeeds'
local CoolingModeIndependentCompressorSpeeds = require 'Compressor/CoolingModeIndependentCompressorSpeeds'
local CoolingModeDependentCompressorSpeeds = require 'Compressor/CoolingModeDependentCompressorSpeeds'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('compressor_speeds', function()
  local compressor_speeds = CompressorSpeeds(core_mock)
  local coolingModeIndependentCompressorSpeeds = CoolingModeIndependentCompressorSpeeds(core_mock)
  local coolingModeDependentCompressorSpeeds = CoolingModeDependentCompressorSpeeds(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      startup_speed_frequency_in_hz = 40,
      super_low_speed_frequency_in_hz = 10,
      super_high_speed_frequency_in_hz = 200,
    }, overrides or {})
  end

  it('should require all arguments for cooling mode independent speeds', function()
    should_require_args(compressor_speeds, generate_config({
      cooling_mode_dependent = false,
      cooling_mode_independent_compressor_speeds = TypedString('cooling_mode_independent_compressor_speeds')
    }))
  end)

  it('should require all arguments for cooling dependent speeds', function()
    should_require_args(compressor_speeds, generate_config({
      cooling_mode_dependent = true,
      cooling_mode_dependent_compressor_speeds = TypedString('cooling_mode_dependent_compressor_speeds')
    }))
  end)

  it('should assert if startup_speed_frequency_in_hz is not in range', function()
    should_fail_with('startup_speed_frequency_in_hz=-1 must be in [0, 255]', function()
      compressor_speeds(generate_config({
        cooling_mode_dependent = false,
        cooling_mode_independent_compressor_speeds = TypedString('cooling_mode_independent_compressor_speeds'),
        startup_speed_frequency_in_hz = -1
      }))
    end)
  end)

  it('should assert if super_low_speed_frequency_in_hz is not in range', function()
    should_fail_with('super_low_speed_frequency_in_hz=-1 must be in [0, 255]', function()
      compressor_speeds(generate_config({
        cooling_mode_dependent = false,
        cooling_mode_independent_compressor_speeds = TypedString('cooling_mode_independent_compressor_speeds'),
        super_low_speed_frequency_in_hz = -1
      }))
    end)
  end)

  it('should assert if super_high_speed_frequency_in_hz is not in range', function()
    should_fail_with('super_high_speed_frequency_in_hz=-1 must be in [0, 255]', function()
      compressor_speeds(generate_config({
        cooling_mode_dependent = false,
        cooling_mode_independent_compressor_speeds = TypedString('cooling_mode_independent_compressor_speeds'),
        super_high_speed_frequency_in_hz = -1
      }))
    end)
  end)

  it('should assert if super_high_speed_frequency_in_hz is not in range', function()
    should_fail_with('super_high_speed_frequency_in_hz=-100000 must be in [0, 255]', function()
      compressor_speeds(generate_config({
        cooling_mode_dependent = false,
        cooling_mode_independent_compressor_speeds = TypedString('cooling_mode_independent_compressor_speeds'),
        super_high_speed_frequency_in_hz = -100000
      }))
    end)
  end)

  it('should assert if no independent speed table is given for cooling mode independent compressor speeds', function()
    should_fail_with('missing argument cooling_mode_independent_compressor_speeds', function()
    compressor_speeds(generate_config({
      cooling_mode_dependent = false,
    }))
    end)
  end)

  it('should assert if no dependent speed table is given for cooling mode dependent compressor speeds', function()
    should_fail_with('missing argument cooling_mode_dependent_compressor_speeds', function()
    compressor_speeds(generate_config({
      cooling_mode_dependent = true,
    }))
    end)
  end)

  it('should generate a typed string with the correct data and type compressor_speeds for cooling independent speeds data', function()
    local expected = remove_whitespace([[
        structure(
          bool(false),
          u8(10),
          u8(20),
          u8(200),
          structure(
            u8(10),
            u8(20),
            u8(30)
          )
        )
      ]])

    local actual = compressor_speeds({
      cooling_mode_dependent = false,
      startup_speed_frequency_in_hz = 10,
      super_low_speed_frequency_in_hz = 20,
      super_high_speed_frequency_in_hz = 200,
      cooling_mode_independent_compressor_speeds = coolingModeIndependentCompressorSpeeds({
        low_speed_frequency_in_hz = 10,
        medium_speed_frequency_in_hz = 20,
        high_speed_frequency_in_hz = 30
      })
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('compressor_speeds'))
  end)

  it('should generate a typed string with the correct data and type compressor_speeds for cooling dependent speeds data', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
          u8(10),
          u8(20),
          u8(200),
          structure(
            u8(40),
            u8(41),
            u8(42),
            u8(50),
            u8(51),
            u8(52),
            u8(53),
            u8(54),
            u8(55),
            u8(60),
            u8(61),
            u8(62),
            u8(63),
            u8(64),
            u8(65),
            u8(70),
            u8(71),
            u8(72),
            u8(80),
            u8(81),
            u8(82)
          )
        )
    ]])

    local actual = compressor_speeds({
      cooling_mode_dependent = true,
      startup_speed_frequency_in_hz = 10,
      super_low_speed_frequency_in_hz = 20,
      super_high_speed_frequency_in_hz = 200,
      cooling_mode_dependent_compressor_speeds = coolingModeDependentCompressorSpeeds({
        low_speed_fresh_food_warm_frequency_in_hz = 40,
        low_speed_fresh_food_mid_frequency_in_hz = 41,
        low_speed_fresh_food_cold_frequency_in_hz = 42,
        low_speed_freezer_warm_frequency_in_hz = 50,
        low_speed_freezer_mid_frequency_in_hz = 51,
        low_speed_freezer_cold_frequency_in_hz = 52,
        low_speed_convertible_compartment_warm_frequency_in_hz = 53,
        low_speed_convertible_compartment_mid_frequency_in_hz = 54,
        low_speed_convertible_compartment_cold_frequency_in_hz = 55,
        medium_speed_fresh_food_warm_frequency_in_hz = 60,
        medium_speed_fresh_food_mid_frequency_in_hz = 61,
        medium_speed_fresh_food_cold_frequency_in_hz = 62,
        medium_speed_freezer_warm_frequency_in_hz = 63,
        medium_speed_freezer_mid_frequency_in_hz = 64,
        medium_speed_freezer_cold_frequency_in_hz = 65,
        medium_speed_convertible_compartment_warm_frequency_in_hz = 70,
        medium_speed_convertible_compartment_mid_frequency_in_hz = 71,
        medium_speed_convertible_compartment_cold_frequency_in_hz = 72,
        high_speed_fresh_food_frequency_in_hz = 80,
        high_speed_freezer_frequency_in_hz = 81,
        high_speed_convertible_compartment_frequency_in_hz = 82
      })
    })
    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('compressor_speeds'))
  end)

  it('should memoize for cooling independent speeds', function()
    should_memoize_calls(compressor_speeds, generate_config({
      cooling_mode_dependent = false,
      cooling_mode_independent_compressor_speeds = TypedString('cooling_mode_independent_compressor_speeds'),
    }))
  end)

  it('should memoize for cooling dependent speeds', function()
    should_memoize_calls(compressor_speeds, generate_config({
      cooling_mode_dependent = true,
      cooling_mode_dependent_compressor_speeds = TypedString('cooling_mode_dependent_compressor_speeds'),
    }))
  end)
end)

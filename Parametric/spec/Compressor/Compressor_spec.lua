local Compressor = require 'Compressor/Compressor'
local CompressorSpeeds = require 'Compressor/CompressorSpeeds'
local CompressorTimes = require 'Compressor/CompressorTimes'
local CoolingModeDependentCompressorSpeeds = require 'Compressor/CoolingModeDependentCompressorSpeeds'
local CoolingModeIndependentCompressorSpeeds = require 'Compressor/CoolingModeIndependentCompressorSpeeds'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('compressor', function()
  local compressorTimes = CompressorTimes(core_mock, false)
  local compressorSpeeds = CompressorSpeeds(core_mock)
  local compressor = Compressor(core_mock)
  local coolingModeDependentCompressorSpeeds = CoolingModeDependentCompressorSpeeds(core_mock)
  local coolingModeIndependentCompressorSpeeds = CoolingModeIndependentCompressorSpeeds(core_mock)


  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      low_ambient_valve_trigger_temperature_in_deg_fx100 = 300,
      compressor_times = TypedString('compressor_times')
    }, overrides or {})
  end

  it('should require all arguments except speeds for single speed compressor', function()
    should_require_args(compressor, generate_config({
      compressor_is_single_speed = true
    }))
  end)

  it('should require all arguments including speeds for variable speed compressor', function()
    should_require_args(compressor, generate_config({
      compressor_is_single_speed = false,
      variable_speed_compressor_relay_is_enabled = true,
      compressor_speeds = TypedString('compressor_speeds')
    }))
  end)

  it('should assert if compressor_is_single_speed is not a boolean', function()
    should_fail_with('compressor_is_single_speed must be of type boolean but is of type number', function()
      compressor(generate_config({
        compressor_is_single_speed = -1
      }))
    end)
  end)

  it('should assert if compressor_is_single_speed is never given', function()
    should_fail_with('missing argument compressor_is_single_speed', function()
      compressor(generate_config({
        compressor_speeds = TypedString('compressor_speeds')
      }))
    end)
  end)

  it('should assert if low_ambient_valve_trigger_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('low_ambient_valve_trigger_temperature_in_deg_fx100=-100000 must be in [-32768, 32767]', function()
      compressor(generate_config({
        compressor_is_single_speed = false,
        variable_speed_compressor_relay_is_enabled = true,
        compressor_speeds = TypedString('compressor_speeds'),
        low_ambient_valve_trigger_temperature_in_deg_fx100= -100000
      }))
    end)
  end)

  it('should assert if variable_speed_compressor_relay_is_enabled is not a boolean', function()
    should_fail_with('variable_speed_compressor_relay_is_enabled must be of type boolean but is of type number', function()
      compressor(generate_config({
        compressor_is_single_speed = false,
        compressor_speeds = TypedString('compressor_speeds'),
        variable_speed_compressor_relay_is_enabled = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data for single speed compressor', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        i16(300),
        bool(false),
       structure(
            u8(1),
            u8(2),
            u8(55),
            u8(2),
            u8(3),
            u8(1)
          )
        )
    ]])

    local actual = compressor({
      compressor_is_single_speed = true,
      low_ambient_valve_trigger_temperature_in_deg_fx100 = 300,
      compressor_times = compressorTimes({
        minimum_off_time_in_minutes = 1,
        minimum_on_time_in_minutes =2,
        startup_on_time_in_seconds  = 55,
        remain_off_after_valve_move_in_minutes = 2,
        remain_off_after_valve_move_low_ambient_in_minutes = 3,
        variable_speed_minimum_run_time_in_minutes = 1
      })

    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('compressor'))
  end)

  it('should generate a typed string with the correct data for variable speed compressor with cooling dependent speeds', function()
    local expected = remove_whitespace([[
   structure(
    bool(false),
    i16(300),
    bool(true),
    structure(
      u8(1),
      u8(2),
      u8(55),
      u8(2),
      u8(3),
      u8(1)
    ),
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
   )
    ]])

    local actual = compressor({
      compressor_is_single_speed = false,
      low_ambient_valve_trigger_temperature_in_deg_fx100 = 300,
      variable_speed_compressor_relay_is_enabled = true,
      compressor_times = compressorTimes({
        minimum_off_time_in_minutes = 1,
        minimum_on_time_in_minutes =2,
        startup_on_time_in_seconds  = 55,
        remain_off_after_valve_move_in_minutes = 2,
        remain_off_after_valve_move_low_ambient_in_minutes = 3,
        variable_speed_minimum_run_time_in_minutes = 1
      }),
      compressor_speeds = compressorSpeeds({
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
    })
    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('compressor'))
  end)


  it('should generate a typed string with the correct data for variable speed compressor with cooling independent speeds', function()
    local expected = remove_whitespace([[
    structure(
      bool(false),
      i16(300),
      bool(true),
      structure(
        u8(1),
        u8(2),
        u8(55),
        u8(2),
        u8(3),
        u8(1)
      ),
      structure(
        bool(false),
        u8(10),
        u8(20),
        u8(200),
        structure(
          u8(80),
          u8(81),
          u8(82)
        )
      )
    )
  ]])

    local actual = compressor({
      compressor_is_single_speed = false,
      low_ambient_valve_trigger_temperature_in_deg_fx100 = 300,
      variable_speed_compressor_relay_is_enabled = true,
      compressor_times = compressorTimes({
        minimum_off_time_in_minutes = 1,
        minimum_on_time_in_minutes =2,
        startup_on_time_in_seconds  = 55,
        remain_off_after_valve_move_in_minutes = 2,
        remain_off_after_valve_move_low_ambient_in_minutes = 3,
        variable_speed_minimum_run_time_in_minutes = 1
      }),
      compressor_speeds = compressorSpeeds({
        cooling_mode_dependent = false,
        startup_speed_frequency_in_hz = 10,
        super_low_speed_frequency_in_hz = 20,
        super_high_speed_frequency_in_hz = 200,
        cooling_mode_independent_compressor_speeds = coolingModeIndependentCompressorSpeeds({
          low_speed_frequency_in_hz = 80,
          medium_speed_frequency_in_hz = 81,
          high_speed_frequency_in_hz = 82
        })
      })
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('compressor'))
  end)

  it('should memoize single speed compressor', function()
    should_memoize_calls(compressor, generate_config({
      compressor_is_single_speed = true
    }))
  end)

  it('should memoize variable speed compressor that is cooling mode independent', function()
    should_memoize_calls(compressor, generate_config({
      compressor_is_single_speed = false,
      variable_speed_compressor_relay_is_enabled = true,
      compressor_speeds = compressorSpeeds({
        cooling_mode_dependent = false,
        super_low_speed_frequency_in_hz = 50,
        super_high_speed_frequency_in_hz = 60,
        startup_speed_frequency_in_hz = 40,
        cooling_mode_independent_compressor_speeds = coolingModeIndependentCompressorSpeeds({
          low_speed_frequency_in_hz = 40,
          medium_speed_frequency_in_hz = 50,
          high_speed_frequency_in_hz = 60
        })
      })
    }))
  end)

  it('should memoize variable speed compressor that is cooling mode dependent', function()
    should_memoize_calls(compressor, generate_config({
      compressor_is_single_speed = false,
      variable_speed_compressor_relay_is_enabled = true,
      compressor_speeds = compressorSpeeds({
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
    }))
  end)

end)

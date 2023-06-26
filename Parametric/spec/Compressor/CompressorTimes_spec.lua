local CompressorTimes = require 'Compressor/CompressorTimes'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('compressor_times', function()
  local compressorTimes = CompressorTimes(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      minimum_off_time_in_minutes = 10,
      minimum_on_time_in_minutes = 5,
      startup_on_time_in_seconds = 45,
      remain_off_after_valve_move_in_minutes = 30,
      remain_off_after_valve_move_low_ambient_in_minutes = 29,
      variable_speed_minimum_run_time_in_minutes = 3,
      sabbath_delay_time_in_seconds = 20
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(compressorTimes, generate_config())
  end)

  it('should assert if minimum_off_time is not in range', function()
    should_fail_with('minimum_off_time_in_minutes=-1 must be in [0, 255]', function()
      compressorTimes(generate_config({
        minimum_off_time_in_minutes = -1
      }))
    end)
  end)

  it('should assert if minimum_on_time is not in range', function()
    should_fail_with('minimum_on_time_in_minutes=-1 must be in [0, 255]', function()
      compressorTimes(generate_config({
        minimum_on_time_in_minutes = -1
      }))
    end)
  end)

  it('should assert if startup_on_time is not in range', function()
    should_fail_with('startup_on_time_in_seconds=-1 must be in [0, 255]', function()
      compressorTimes(generate_config({
        startup_on_time_in_seconds = -1
      }))
    end)
  end)

  it('should assert if remain_off_after_valve_move_in_minutes is not in range', function()
    should_fail_with('remain_off_after_valve_move_in_minutes=-1 must be in [0, 255]', function()
      compressorTimes(generate_config({
        remain_off_after_valve_move_in_minutes = -1
      }))
    end)
  end)

  it('should assert if remain_off_after_valve_move_low_ambient_in_minutes is not in range', function()
    should_fail_with('remain_off_after_valve_move_low_ambient_in_minutes=-1 must be in [0, 255]', function()
      compressorTimes(generate_config({
        remain_off_after_valve_move_low_ambient_in_minutes = -1
      }))
    end)
  end)

  it('should assert if variable_speed_minimum_run_time_in_minutes is not in range if variable speed', function()
    should_fail_with('variable_speed_minimum_run_time_in_minutes=-1 must be in [0, 255]', function()
      compressorTimes(generate_config({
        variable_speed_minimum_run_time_in_minutes = -1
      }))
    end)
  end)

  it('should assert if sabbath_delay_time_in_seconds is not in range', function()
    should_fail_with('sabbath_delay_time_in_seconds=-1 must be in [15, 25]', function()
      compressorTimes(generate_config({
        sabbath_delay_time_in_seconds = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type compressor_times', function()
    local expected = remove_whitespace([[
      structure(
        u8(10),
        u8(5),
        u8(45),
        u8(30),
        u8(29),
        u8(3),
        u8(20))
    ]])

    local actual = compressorTimes({
      minimum_off_time_in_minutes = 10,
      minimum_on_time_in_minutes = 5,
      startup_on_time_in_seconds = 45,
      remain_off_after_valve_move_in_minutes = 30,
      remain_off_after_valve_move_low_ambient_in_minutes = 29,
      variable_speed_minimum_run_time_in_minutes = 3,
      sabbath_delay_time_in_seconds = 20
    })

  end)

  it('should memoize', function()
    should_memoize_calls(compressorTimes, generate_config())
  end)
end)

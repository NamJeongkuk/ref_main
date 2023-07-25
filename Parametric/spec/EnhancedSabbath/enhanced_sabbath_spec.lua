local EnhancedSabbath = require 'EnhancedSabbath/EnhancedSabbath'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('enhancedSabbath', function()
  local enhancedSabbath = EnhancedSabbath(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
        max_time_in_enhanced_sabbath_mode_in_minutes = 5760,
        fresh_food_setpoint_temperature_in_degfx100 = 3700,
        freezer_setpoint_temperature_in_degfx100 = 0,
        number_of_fresh_food_defrosts_before_freezer_defrost = 3,
        min_time_between_temperature_averaging_in_minutes = 2,
        fresh_food_stage_time_in_minutes = 1,
        freezer_stage_time_in_minutes = 2,
        off_stage_time_in_minutes = 3
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(enhancedSabbath, generate_config())
  end)

  it('should assert if max_time_in_enhanced_sabbath_mode_in_minutes is not in range', function()
    should_fail_with('max_time_in_enhanced_sabbath_mode_in_minutes=-1 must be in [0, 65535]', function()
      enhancedSabbath(generate_config({
        max_time_in_enhanced_sabbath_mode_in_minutes = -1
      }))
    end)
  end)

  it('should assert if fresh_food_setpoint_temperature_in_degfx100 is not a number', function()
    should_fail_with('fresh_food_setpoint_temperature_in_degfx100 must be of type number but is of type string', function()
      enhancedSabbath(generate_config({
        fresh_food_setpoint_temperature_in_degfx100 = "not a number"
      }))
    end)
  end)

  it('should assert if freezer_setpoint_temperature_in_degfx100 is not a number', function()
    should_fail_with('freezer_setpoint_temperature_in_degfx100 must be of type number but is of type string', function()
      enhancedSabbath(generate_config({
        freezer_setpoint_temperature_in_degfx100 = "not a number"
      }))
    end)
  end)

  it('should assert if number_of_fresh_food_defrosts_before_freezer_defrost is not in range', function()
    should_fail_with('number_of_fresh_food_defrosts_before_freezer_defrost=-1 must be in [0, 255]', function()
      enhancedSabbath(generate_config({
        number_of_fresh_food_defrosts_before_freezer_defrost = -1
      }))
    end)
  end)

  it('should assert if min_time_between_temperature_averaging_in_minutes is not in range', function()
    should_fail_with('min_time_between_temperature_averaging_in_minutes=-1 must be in [0, 255]', function()
      enhancedSabbath(generate_config({
        min_time_between_temperature_averaging_in_minutes = -1
      }))
    end)
  end)

  it('should assert if fresh_food_stage_time_in_minutes is not in range', function()
    should_fail_with('fresh_food_stage_time_in_minutes=-1 must be in [0, 255]', function()
      enhancedSabbath(generate_config({
        fresh_food_stage_time_in_minutes = -1
      }))
    end)
  end)

  it('should assert if freezer_stage_time_in_minutes is not in range', function()
    should_fail_with('freezer_stage_time_in_minutes=-1 must be in [0, 255]', function()
      enhancedSabbath(generate_config({
        freezer_stage_time_in_minutes = -1
      }))
    end)
  end)

  it('should assert if off_stage_time_in_minutes is not in range', function()
    should_fail_with('off_stage_time_in_minutes=-1 must be in [0, 255]', function()
      enhancedSabbath(generate_config({
        off_stage_time_in_minutes = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type enhancedSabbath', function()
    local expected = remove_whitespace([[
      structure(
        u16(5760),
        i16(3700),
        i16(0),
        u8(3),
        u8(2),
        u8(1),
        u8(2),
        u8(3)
      )
    ]])

    local actual = enhancedSabbath(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('enhancedSabbath'))
  end)

  it('should memoize', function()
    should_memoize_calls(enhancedSabbath, generate_config())
  end)

end)

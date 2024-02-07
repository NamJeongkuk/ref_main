local FanCareAboutCoolingModeSpeedTable = require 'Fan/FanCareAboutCoolingModeSpeedTable'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local DutyCycle = require 'Fan/DutyCycle'
local Rpm = require 'Fan/Rpm'
local TypedString = require 'lua-common'.util.TypedString

describe('FanCareAboutCoolingModeSpeedTable', function()
  local fan_care_about_cooling_mode_speed_table = FanCareAboutCoolingModeSpeedTable(core_mock)
  local duty_cycle = DutyCycle(core_mock)
  local rpm = Rpm(core_mock)

  local function generate_config(care_about_freezer_setpoint, overrides)
    if care_about_freezer_setpoint == true then
      return require 'lua-common'.table.merge({
        care_about_freezer_setpoint = true,
        super_low_speed = duty_cycle({duty_cycle = 10}),
        low_speed_fresh_food = rpm({rpm = 1000}),
        low_speed_freezer_cold_setpoint = rpm({rpm = 1100}),
        low_speed_freezer_medium_setpoint = duty_cycle({duty_cycle = 12}),
        low_speed_freezer_warm_setpoint = duty_cycle({duty_cycle = 13}),
        low_speed_convertible_compartment_cold_setpoint = rpm({rpm = 1200}),
        low_speed_convertible_compartment_medium_setpoint = rpm({rpm = 1300}),
        low_speed_convertible_compartment_warm_setpoint = duty_cycle({duty_cycle = 14}),
        medium_speed_fresh_food = duty_cycle({duty_cycle = 15}),
        medium_speed_freezer = rpm({rpm = 1400}),
        medium_speed_convertible_compartment = rpm({rpm = 1500}),
        high_speed_fresh_food = duty_cycle({duty_cycle = 16}),
        high_speed_freezer = rpm({rpm = 1600}),
        high_speed_convertible_compartment = rpm({rpm = 1700}),
        super_high_speed = duty_cycle({duty_cycle = 18})
      }, overrides or {})
    else
      return require 'lua-common'.table.merge({
        care_about_freezer_setpoint = false,
        super_low_speed = duty_cycle({duty_cycle = 10}),
        low_speed_fresh_food = rpm({rpm = 1000}),
        low_speed_freezer = rpm({rpm = 1000}),
        medium_speed_fresh_food = duty_cycle({duty_cycle = 15}),
        medium_speed_freezer = rpm({rpm = 1400}),
        high_speed_fresh_food = duty_cycle({duty_cycle = 16}),
        high_speed_freezer = rpm({rpm = 1600}),
        super_high_speed = duty_cycle({duty_cycle = 18})
      }, overrides or {})
    end

  end

  it('should require all arguments', function()
    should_require_args(fan_care_about_cooling_mode_speed_table, generate_config())
  end)

  it('should assert if care_about_freezer_setpoint fields are not a speed', function()
    should_fail_with('super_low_speed must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        super_low_speed = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_fresh_food must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_fresh_food = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_freezer_cold_setpoint must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_freezer_cold_setpoint = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_freezer_medium_setpoint must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_freezer_medium_setpoint = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_freezer_warm_setpoint must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_freezer_warm_setpoint = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_convertible_compartment_cold_setpoint must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_convertible_compartment_cold_setpoint = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_convertible_compartment_medium_setpoint must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_convertible_compartment_medium_setpoint = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_convertible_compartment_warm_setpoint must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_convertible_compartment_warm_setpoint = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('medium_speed_fresh_food must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        medium_speed_fresh_food = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('medium_speed_freezer must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        medium_speed_freezer = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('medium_speed_convertible_compartment must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        medium_speed_convertible_compartment = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_speed_fresh_food must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        high_speed_fresh_food = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_speed_freezer must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        high_speed_freezer = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_speed_convertible_compartment must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        high_speed_convertible_compartment = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('super_high_speed must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        super_high_speed = TypedString('blah', 'some_blah')
      }))
    end)
  end)

  it('should assert if non care_about_freezer_setpoint fields are used when care_about_freezer_setpoint is true', function()
    should_fail_with('unexpected argument low_speed', function()
      fan_care_about_cooling_mode_speed_table(generate_config(true, {
        low_speed_freezer = rpm({rpm = 300})
      }))
    end)
  end)

  it('should assert if care_about_freezer_setpoint fields are used when care_about_freezer_setpoint is false', function()
    should_fail_with('unexpected argument low_speed_freezer_cold_setpoint', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_freezer_cold_setpoint = rpm({rpm = 300})
      }))
    end)

    should_fail_with('unexpected argument low_speed_freezer_medium_setpoint', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_freezer_medium_setpoint = rpm({rpm = 300})
      }))
    end)

    should_fail_with('unexpected argument low_speed_freezer_warm_setpoint', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_freezer_warm_setpoint = rpm({rpm = 300})
      }))
    end)

    should_fail_with('unexpected argument low_speed_convertible_compartment_cold_setpoint', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_convertible_compartment_cold_setpoint = rpm({rpm = 300})
      }))
    end)

    should_fail_with('unexpected argument low_speed_convertible_compartment_medium_setpoint', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_convertible_compartment_medium_setpoint = rpm({rpm = 300})
      }))
    end)

    should_fail_with('unexpected argument low_speed_convertible_compartment_warm_setpoint', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_convertible_compartment_warm_setpoint = rpm({rpm = 300})
      }))
    end)

    should_fail_with('unexpected argument medium_speed_convertible_compartment', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        medium_speed_convertible_compartment = rpm({rpm = 300})
      }))
    end)

    should_fail_with('unexpected argument high_speed_convertible_compartment', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        high_speed_convertible_compartment = rpm({rpm = 300})
      }))
    end)
  end)

  it('should assert if non care_about_freezer_setpoint and non care_about_ambient_temperature fields are not a speed', function()
    should_fail_with('super_low_speed must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        super_low_speed = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_fresh_food must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_fresh_food = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('low_speed_freezer must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        low_speed_freezer = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('medium_speed_fresh_food must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        medium_speed_fresh_food = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('medium_speed_freezer must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        medium_speed_freezer = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_speed_fresh_food must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        high_speed_fresh_food = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('high_speed_freezer must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        high_speed_freezer = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('super_high_speed must be a typed string with type speed, has type(s) blah', function()
      fan_care_about_cooling_mode_speed_table(generate_config(false, {
        super_high_speed = TypedString('blah', 'some_blah')
      }))
    end)
  end)

  it('should generate a typed string with the correct data with care_about_freezer_setpoint flag set to true', function()
    local expected = remove_whitespace([[
      structure(
        bool(true),
        structure(
          u8(0),
          u8(0),
          u8(10),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1000)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1100)
        ),
        structure(
          u8(0),
          u8(0),
          u8(12),
          u8(0)
        ),
        structure(
          u8(0),
          u8(0),
          u8(13),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1200)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1300)
        ),
        structure(
          u8(0),
          u8(0),
          u8(14),
          u8(0)
        ),
        structure(
          u8(0),
          u8(0),
          u8(15),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1400)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1500)
        ),
        structure(
          u8(0),
          u8(0),
          u8(16),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1600)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1700)
        ),
        structure(
          u8(0),
          u8(0),
          u8(18),
          u8(0)
        ))
     ]])

    local actual = fan_care_about_cooling_mode_speed_table({
      care_about_freezer_setpoint = true,
      super_low_speed = duty_cycle({duty_cycle = 10}),
      low_speed_fresh_food = rpm({rpm = 1000}),
      low_speed_freezer_cold_setpoint = rpm({rpm = 1100}),
      low_speed_freezer_medium_setpoint = duty_cycle({duty_cycle = 12}),
      low_speed_freezer_warm_setpoint = duty_cycle({duty_cycle = 13}),
      low_speed_convertible_compartment_cold_setpoint = rpm({rpm = 1200}),
      low_speed_convertible_compartment_medium_setpoint = rpm({rpm = 1300}),
      low_speed_convertible_compartment_warm_setpoint = duty_cycle({duty_cycle = 14}),
      medium_speed_fresh_food = duty_cycle({duty_cycle = 15}),
      medium_speed_freezer = rpm({rpm = 1400}),
      medium_speed_convertible_compartment = rpm({rpm = 1500}),
      high_speed_fresh_food = duty_cycle({duty_cycle = 16}),
      high_speed_freezer = rpm({rpm = 1600}),
      high_speed_convertible_compartment = rpm({rpm = 1700}),
      super_high_speed = duty_cycle({duty_cycle = 18})
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fan_care_about_cooling_mode_speed_table'))
  end)

  it('should generate a typed string with the correct data with care_about_freezer_setpoint flag set to false', function()
    local expected = remove_whitespace([[
      structure(
        bool(false),
        structure(
          u8(0),
          u8(0),
          u8(10),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1000)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1000)
        ),
        structure(
          u8(0),
          u8(0),
          u8(15),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1400)
        ),
        structure(
          u8(0),
          u8(0),
          u8(16),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1600)
        ),
        structure(
          u8(0),
          u8(0),
          u8(18),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(0)
        ))
     ]])

    local actual = fan_care_about_cooling_mode_speed_table({
      care_about_freezer_setpoint = false,
      super_low_speed = duty_cycle({duty_cycle = 10}),
      low_speed_fresh_food = rpm({rpm = 1000}),
      low_speed_freezer = rpm({rpm = 1000}),
      medium_speed_fresh_food = duty_cycle({duty_cycle = 15}),
      medium_speed_freezer = rpm({rpm = 1400}),
      high_speed_fresh_food = duty_cycle({duty_cycle = 16}),
      high_speed_freezer = rpm({rpm = 1600}),
      super_high_speed = duty_cycle({duty_cycle = 18})
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fan_care_about_cooling_mode_speed_table'))
  end)

  it('should memoize', function()
    should_memoize_calls(fan_care_about_cooling_mode_speed_table, generate_config())
  end)

end)

local FanSpeedTable = require 'Fan/FanSpeedTable'
local DutyCycle = require 'Fan/DutyCycle'
local Rpm = require 'Fan/Rpm'
local TypedString = require 'lua-common'.util.TypedString
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('FanSpeedTable', function()
  local fan_speed_table = FanSpeedTable(core_mock)
  local duty_cycle = DutyCycle(core_mock)
  local rpm = Rpm(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      super_low_speed = duty_cycle({duty_cycle = 10}),
      low_speed = rpm({rpm = 1000}),
      medium_speed = duty_cycle({duty_cycle = 50}),
      high_speed = rpm({rpm = 1800}),
      super_high_speed = duty_cycle({duty_cycle = 100})
    }, overrides or {})
  end

  local function generate_single_speed_config(overrides)
    return require 'lua-common'.table.merge({
      single_speed_option = true,
      on_speed = rpm({rpm = 1000})
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(fan_speed_table, generate_config())
  end)

  it('should assert if fields are not a speed', function()
    should_fail_with('super_low_speed must be a typed string with type speed, has type(s) blah', function()
      fan_speed_table(generate_config({ super_low_speed = TypedString('blah', 'some_blah') }))
    end)

    should_fail_with('low_speed must be a typed string with type speed, has type(s) blah', function()
      fan_speed_table(generate_config({ low_speed = TypedString('blah', 'some_blah') }))
    end)

    should_fail_with('medium_speed must be a typed string with type speed, has type(s) blah', function()
      fan_speed_table(generate_config({ medium_speed = TypedString('blah', 'some_blah') }))
    end)

    should_fail_with('high_speed must be a typed string with type speed, has type(s) blah', function()
      fan_speed_table(generate_config({ high_speed = TypedString('blah', 'some_blah') }))
    end)

    should_fail_with('super_high_speed must be a typed string with type speed, has type(s) blah', function()
      fan_speed_table(generate_config({ super_high_speed = TypedString('blah', 'some_blah') }))
    end)
  end)

  it('should assert if on_speed is defined if single_speed_option is not present', function()
    should_fail_with('unexpected argument on_speed', function()
      fan_speed_table(generate_config({ on_speed = rpm({rpm = 300}) }))
    end)
  end)

  it('should assert if non single speed options are defined when single_speed_option is true', function()
    should_fail_with('unexpected argument super_low_speed', function()
      fan_speed_table(generate_single_speed_config({ super_low_speed = rpm({rpm = 300}) }))
    end)

    should_fail_with('unexpected argument low_speed', function()
      fan_speed_table(generate_single_speed_config({ low_speed = rpm({rpm = 300}) }))
    end)

    should_fail_with('unexpected argument medium_speed', function()
      fan_speed_table(generate_single_speed_config({ medium_speed = rpm({rpm = 300}) }))
    end)

    should_fail_with('unexpected argument high_speed', function()
      fan_speed_table(generate_single_speed_config({ high_speed = rpm({rpm = 300}) }))
    end)

    should_fail_with('unexpected argument super_high_speed', function()
      fan_speed_table(generate_single_speed_config({ super_high_speed = rpm({rpm = 300}) }))
    end)
  end)

  it('should generate a typed string with the correct data and type fan_speed_table', function()
    local expected = remove_whitespace([[
      structure(
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
          u8(0),
          u8(0),
          u8(50),
          u8(0)
        ),
        structure(
          u8(1),
          u8(0),
          u16(1800)
        ),
        structure(
          u8(0),
          u8(0),
          u8(100),
          u8(0)
        )
      )
      ]])

    local actual = fan_speed_table({
      super_low_speed = duty_cycle({duty_cycle = 10}),
      low_speed = rpm({rpm = 1000}),
      medium_speed = duty_cycle({duty_cycle = 50}),
      high_speed = rpm({rpm = 1800}),
      super_high_speed = duty_cycle({duty_cycle = 100})
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fan_speed_table'))
  end)

  it('should generate a typed string with the correct data and type fan_speed_table when single_speed_option is true', function()
    local expected = remove_whitespace([[
      structure(
        bool(false),
        structure(
          u8(1),
          u8(0),
          u16(1000)
        )
      )
      ]])

    local actual = fan_speed_table({
      single_speed_option = true,
      on_speed = rpm({rpm = 1000})
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fan_speed_table'))
  end)

  it('should memoize', function()
    should_memoize_calls(fan_speed_table, generate_config())
  end)
end)

local DamperHeater = require 'Damper/DamperHeater'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('DamperHeater', function()
  local damper_heater = DamperHeater(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      damper_heater_on_when_defrost_heater_is_on = false,
      damper_heater_defrost_on_time_in_minutes = 255
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(damper_heater, generate_config())
  end)

  it('should assert if damper_heater_on_when_defrost_heater_is_on is not a boolean', function()
    should_fail_with('damper_heater_on_when_defrost_heater_is_on must be of type boolean but is of type number', function()
      damper_heater(generate_config({
         damper_heater_on_when_defrost_heater_is_on = 1
      }))
    end)
  end)

  it('should assert if damper_heater_defrost_on_time_in_minutes is not in range', function()
    should_fail_with('damper_heater_defrost_on_time_in_minutes=256 must be in [0, 255]', function()
      damper_heater(generate_config({
        damper_heater_defrost_on_time_in_minutes = 256
      }))
    end)
  end)

  it('should assert if damper_heater_defrost_on_time_in_minutes is not zero when damper_heater_on_when_defrost_heater_is_on is true', function()
    should_fail_with('damper_heater_defrost_on_time_in_minutes=10 must be equal to 0', function()
      damper_heater(generate_config({
        damper_heater_on_when_defrost_heater_is_on = true,
        damper_heater_defrost_on_time_in_minutes = 10
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for single damper', function()
    local expected = remove_whitespace([[
      structure(
        bool(false),
        u8(10)
      )
    ]])

    local actual = damper_heater({
      damper_heater_on_when_defrost_heater_is_on = false,
      damper_heater_defrost_on_time_in_minutes = 10
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('damper_heater'))
  end)

  it('should memoize', function()
    should_memoize_calls(damper_heater, generate_config())
  end)
end)

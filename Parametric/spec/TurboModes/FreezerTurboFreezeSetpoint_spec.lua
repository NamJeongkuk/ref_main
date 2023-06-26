local FreezerTurboFreezeSetpoint = require 'TurboModes/FreezerTurboFreezeSetpoint'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('FreezerTurboFreezeSetpoint', function()
  local freezer_turbo_freeze_setpoint = FreezerTurboFreezeSetpoint(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      turbo_freeze_setpoint_in_degfx100 = 3500,
      turbo_freeze_on_time_in_minutes = 360
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(freezer_turbo_freeze_setpoint, generate_config())
  end)

  it('should assert if turbo_freeze_setpoint_in_degfx100 is not in range', function()
    should_fail_with('turbo_freeze_setpoint_in_degfx100=32768 must be in [-32768, 32767]', function()
      freezer_turbo_freeze_setpoint(generate_config({
        turbo_freeze_setpoint_in_degfx100 = 32768
      }))
    end)
  end)

  it('should assert if turbo_freeze_on_time_in_minutes is not in range', function()
    should_fail_with('turbo_freeze_on_time_in_minutes=65536 must be in [0, 65535]', function()
      freezer_turbo_freeze_setpoint(generate_config({
         turbo_freeze_on_time_in_minutes = 65536
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for freezer turbo freeze setpoint', function()
    local expected = remove_whitespace([[
      structure(
        i16(3500),
        u16(360)
      )
    ]])

    local actual = freezer_turbo_freeze_setpoint(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('freezer_turbo_freeze_setpoint'))
  end)

  it('should memoize', function()
    should_memoize_calls(freezer_turbo_freeze_setpoint, generate_config())
  end)
end)

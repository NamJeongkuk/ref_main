local FanFault = require 'Fan/FanFault'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('fanFault', function()
  local fanFault = FanFault(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      fan_missed_target_fault_timeout_in_seconds = 11,
      missing_fan_feedback_fault_timeout_in_seconds = 12,
      feedback_present_when_fan_is_off_timeout_in_seconds = 13
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(fanFault, generate_config())
  end)

  it('should assert if fan_missed_target_fault_timeout_in_seconds is not in range', function()
    should_fail_with('fan_missed_target_fault_timeout_in_seconds=70000 must be in [0, 65535]', function()
      fanFault(generate_config({
        fan_missed_target_fault_timeout_in_seconds = 70000
      }))
    end)
  end)

  it('should assert if missing_fan_feedback_fault_timeout_in_seconds is not in range', function()
    should_fail_with('missing_fan_feedback_fault_timeout_in_seconds=70000 must be in [0, 65535]', function()
      fanFault(generate_config({
        missing_fan_feedback_fault_timeout_in_seconds = 70000
      }))
    end)
  end)

  it('should assert if feedback_present_when_fan_is_off_timeout_in_seconds is not in range', function()
    should_fail_with('feedback_present_when_fan_is_off_timeout_in_seconds=70000 must be in [0, 65535]', function()
      fanFault(generate_config({
        feedback_present_when_fan_is_off_timeout_in_seconds = 70000
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type fanFault', function()
    local expected = remove_whitespace([[
      structure(
        u16(11),
        u16(12),
        u16(13))
      ]])

    local actual = fanFault({
      fan_missed_target_fault_timeout_in_seconds = 11,
      missing_fan_feedback_fault_timeout_in_seconds = 12,
      feedback_present_when_fan_is_off_timeout_in_seconds = 13
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('fan_fault'))
  end)

  it('should memoize', function()
    should_memoize_calls(fanFault, generate_config())
  end)

end)

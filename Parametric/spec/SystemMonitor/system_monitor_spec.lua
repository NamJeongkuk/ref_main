local SystemMonitor = require 'SystemMonitor/SystemMonitor'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('systemMonitor', function()
  local systemMonitor = SystemMonitor(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      periodic_nv_update_in_minutes = 60,
      periodic_nv_update_defrost_heater_on_time_in_minutes = 15
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(systemMonitor, generate_config())
  end)

  it('should assert if periodic_nv_update_in_minutes is not in range', function()
    should_fail_with('periodic_nv_update_in_minutes=-1 must be in [0, 255]', function()
      systemMonitor(generate_config({
        periodic_nv_update_in_minutes = -1
      }))
    end)
  end)

  it('should assert if periodic_nv_update_defrost_heater_on_time_in_minutes is not in range', function()
    should_fail_with('periodic_nv_update_defrost_heater_on_time_in_minutes=-1 must be in [0, 255]', function()
      systemMonitor(generate_config({
        periodic_nv_update_defrost_heater_on_time_in_minutes = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type systemMonitor', function()
    local expected = remove_whitespace([[
    structure(
      u8(60),
      u8(15))
    ]])

    local actual = systemMonitor({
      periodic_nv_update_in_minutes = 60,
      periodic_nv_update_defrost_heater_on_time_in_minutes = 15
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('systemMonitor'))
  end)

  it('should memoize', function()
    should_memoize_calls(systemMonitor, generate_config())
  end)

end)

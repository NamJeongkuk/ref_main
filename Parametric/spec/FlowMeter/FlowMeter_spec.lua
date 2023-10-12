local FlowMeter = require 'FlowMeter/FlowMeter'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('FlowMeter', function()
  local flow_meter = FlowMeter(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      flow_meter_oz_per_pulseX100000 = 6238,
      flow_meter_offset_ozX100 = 25
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(flow_meter, generate_config())
  end)

  it('should assert if flow_meter_oz_per_pulseX100000 is not in range', function()
    should_fail_with('flow_meter_oz_per_pulseX100000=-1 must be in [0, 65535]', function()
      flow_meter(generate_config({
        flow_meter_oz_per_pulseX100000 = -1
      }))
    end)
  end)

  it('should assert if flow_meter_offset_ozX100 is not in range', function()
    should_fail_with('flow_meter_offset_ozX100=128 must be in [-128, 127]', function()
      flow_meter(generate_config({
        flow_meter_offset_ozX100 = 128
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type flow_meter', function()
    local expected = remove_whitespace([[
      structure(
        u16(6238),
        i8(25)
      )
      ]])

    local actual = flow_meter({
      flow_meter_oz_per_pulseX100000 = 6238,
      flow_meter_offset_ozX100 = 25
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('flow_meter'))
  end)

  it('should memoize', function()
    should_memoize_calls(flow_meter, generate_config())
  end)
end)

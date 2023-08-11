local Gridline = require 'Grid/Gridline'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local grid_line_correction = require 'Grid.GridLineCorrection'

describe('gridline', function()
  local gridline = Gridline(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
        delta_in_degfx100 = 450,
        correction = 'offset'
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(gridline, generate_config())
  end)

  it('should assert if delta_in_degfx100 is not in range', function()
    should_fail_with('delta_in_degfx100=100000 must be in [-32768, 32767]', function()
        gridline(generate_config({
            delta_in_degfx100 = 100000
      }))
    end)
  end)

  it('should assert if correction is not in range', function()
    should_fail_with("correction=-1 must be in the set { 'offset', 'adjusted_setpoint' }", function()
        gridline(generate_config({
          correction = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type gridline', function()
    local expected = remove_whitespace([[
        structure(
        i16(450),
        u8(]] .. grid_line_correction.offset ..[[))
    ]])

    local actual = gridline({
        delta_in_degfx100 = 450,
        correction = 'offset'
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('gridline'))
  end)

  it('should memoize', function()
    should_memoize_calls(gridline, generate_config())
  end)

end)

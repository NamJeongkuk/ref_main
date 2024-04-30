local Gridline = require 'Grid/Gridline'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local grid_line_correction = require 'Grid.GridLineCorrection'
local TypedString = require 'lua-common'.util.TypedString

describe('Gridline', function()
  local gridline = Gridline(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      delta_in_degfx100 = 450,
      correction = grid_line_correction.offset,
      cross_ambient_hysteresis_adjustment = TypedString('cross_ambient_hysteresis_adjustment', 'cross_ambient_hysteresis_adjustment')
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
    should_fail_with("correction=-1 is not in the grid_line_correction enumeration, allowable values: adjusted_setpoint, offset", function()
      gridline(generate_config({
        correction = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('cross_ambient_hysteresis_adjustment must be a typed string with type cross_ambient_hysteresis_adjustment, but is a number', function()
      gridline(generate_config({
        cross_ambient_hysteresis_adjustment = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type gridline', function()
    local expected = remove_whitespace([[
        structure(
          i16(450),
          u8(]] .. grid_line_correction.offset ..[[),
          pointer(cross_ambient_hysteresis_adjustment)
        )
      ]])

    local actual = gridline(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('gridline'))
  end)

  it('should memoize', function()
    should_memoize_calls(gridline, generate_config())
  end)

end)

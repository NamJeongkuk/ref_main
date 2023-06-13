local Grid = require 'Grid/Grid'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local grid_id_type = require 'Grid.GridIdType'

describe('grid', function()
  local grid = Grid(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      grid_id = 'grid_single_evap',
      grid_deltas = TypedString('grid_deltas', 'grid_deltas'),
      grid_invalid_freezer_thermistor_fallback_row = 3,
      grid_invalid_fresh_food_thermistor_fallback_column = 1,
      fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = -20,
      fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 500,
      grid_periodic_run_rate_in_msec = 1000
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(grid, generate_config())
  end)

  it('should require grid_id to be a valid grid id', function()
    should_fail_with("must be in the set { 'grid_single_evap', 'grid_dual_evap', 'grid_triple_evap' }", function()
      grid(generate_config({
        grid_id = 'not a valid grid id'
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('grid_deltas must be a typed string with type grid_deltas, but is a number', function()
      grid(generate_config({
        grid_deltas = -1
      }))
    end)
  end)

  it('should assert if fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf is not in range', function()
    should_fail_with('fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf=999999 must be in [0, 65535]', function()
      grid(generate_config({
        fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 999999
      }))
    end)
  end)

  it('should assert if fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 is not in range', function()
    should_fail_with('fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100=999999 must be in [-32768, 32767]', function()
      grid(generate_config({
        fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = 999999
      }))
    end)
  end)

  it('should assert if grid_invalid_freezer_thermistor_fallback_row is not in range', function()
    should_fail_with('grid_invalid_freezer_thermistor_fallback_row=-1 must be in [0, 255]', function()
      grid(generate_config({
        grid_invalid_freezer_thermistor_fallback_row = -1
      }))
    end)
  end)

  it('should assert if grid_invalid_fresh_food_thermistor_fallback_column is not in range', function()
    should_fail_with('grid_invalid_fresh_food_thermistor_fallback_column=-1 must be in [0, 255]', function()
      grid(generate_config({
        grid_invalid_fresh_food_thermistor_fallback_column = -1
      }))
    end)
  end)

  it('should assert if grid_periodic_run_rate_in_msec is not in range', function()
    should_fail_with('grid_periodic_run_rate_in_msec=1000000 must be in [0, 65535]', function()
      grid(generate_config({
        grid_periodic_run_rate_in_msec = 1000000
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type grid', function()
    local expected = remove_whitespace([[
        structure(
        u8(]] .. grid_id_type.grid_single_evap .. [[),
        pointer(grid_deltas),
        u8(3),
        u8(1),
        i16(-20),
        u16(500),
        u16(1000)
      )
    ]])

    local actual = grid(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('grid'))
  end)

  it('should memoize', function()
    should_memoize_calls(grid, generate_config())
  end)

end)

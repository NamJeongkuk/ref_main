local GridDeltas = require 'Grid.GridDeltas'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('GridDeltas', function()
  local grid_deltas = GridDeltas(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      deltas = {
        fresh_food_grid_deltas = {
          fresh_food_no_freeze_limit_in_degfx100 = TypedString('gridline', 'fresh_food_no_freeze_limit_in_degfx100'),
          fresh_food_low_hyst_in_degfx100 = TypedString('gridline', 'fresh_food_low_hyst_in_degfx100'),
          fresh_food_set_point_delta_in_degfx100 = TypedString('gridline', 'fresh_food_set_point_delta_in_degfx100'),
          fresh_food_high_hyst_in_degfx100 = TypedString('gridline', 'fresh_food_high_hyst_in_degfx100'),
          fresh_food_extra_high_hyst_in_degfx100 = TypedString('gridline', 'fresh_food_extra_high_hyst_in_degfx100'),
          fresh_food_super_high_hyst_in_degfx100 = TypedString('gridline', 'fresh_food_super_high_hyst_in_degfx100')
        },
        freezer_grid_deltas = {
          freezer_low_hyst_in_degfx100 = TypedString('gridline', 'freezer_low_hyst_in_degfx100'),
          freezer_setpoint_delta_in_degfx100 = TypedString('gridline', 'freezer_setpoint_delta_in_degfx100'),
          freezer_high_hyst_in_degfx100 = TypedString('gridline', 'freezer_high_hyst_in_degfx100'),
          freezer_extra_high_hyst_in_degfx100 = TypedString('gridline', 'freezer_extra_high_hyst_in_degfx100'),
          freezer_super_high_hyst_in_degfx100 = TypedString('gridline', 'freezer_super_high_hyst_in_degfx100'),
          freezer_extreme_high_hyst_in_degfx100 = TypedString('gridline', 'freezer_extreme_high_hyst_in_degfx100')
        }
      }
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(grid_deltas, generate_config())
  end)

  it('should fail with missing arguments', function()
    should_fail_with('missing argument deltas.freezer_grid_deltas', function()
      local config = generate_config({deltas = {fresh_food_grid_deltas = {}}})
      grid_deltas(config)
    end)
  end)

  it('should fail with missing arguments', function()
    should_fail_with('missing argument deltas.fresh_food_grid_deltas', function()
      local config = generate_config({deltas = {freezer_grid_deltas = {}}})
      grid_deltas(config)
    end)
  end)

  it('should generate a typed string with the correct data and type grid_deltas', function()
    local expected = remove_whitespace([[
        structure(
          u8(2),
          pointer(
            structure(
              structure(
                u8(6),
                pointer(
                  structure(
                    fresh_food_no_freeze_limit_in_degfx100,
                    fresh_food_low_hyst_in_degfx100,
                    fresh_food_set_point_delta_in_degfx100,
                    fresh_food_high_hyst_in_degfx100,
                    fresh_food_extra_high_hyst_in_degfx100,
                    fresh_food_super_high_hyst_in_degfx100
                  )
                )
              ),
              structure(
                u8(6),
                pointer(
                  structure(
                    freezer_low_hyst_in_degfx100,
                    freezer_setpoint_delta_in_degfx100,
                    freezer_high_hyst_in_degfx100,
                    freezer_extra_high_hyst_in_degfx100,
                    freezer_super_high_hyst_in_degfx100,
                    freezer_extreme_high_hyst_in_degfx100
                  )
                )
              )
            )
          )
        )
    ]])

    local actual = grid_deltas(generate_config())

    assert(actual.is_of_type('grid_deltas'))
    assert.equals(expected, remove_whitespace(tostring(actual)))

  end)

  it('should memoize', function()
    should_memoize_calls(grid_deltas, generate_config())
  end)

end)

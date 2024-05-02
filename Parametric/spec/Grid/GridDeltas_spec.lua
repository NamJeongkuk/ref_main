local GridDeltas = require 'Grid.GridDeltas'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('GridDeltas', function()
  local grid_deltas = GridDeltas(core_mock)

  local function generate_one_dimensional_config(overrides)
    return require 'lua-common'.table.merge({
      deltas = {
        grid_deltas = {
          mode_1 = {
            gridline_1 = TypedString('gridline', 'feature_pan_mode_1_gridline_1'),
            gridline_2 = TypedString('gridline', 'feature_pan_mode_1_gridline_2'),
            gridline_3 = TypedString('gridline', 'feature_pan_mode_1_gridline_3'),
            gridline_4 = TypedString('gridline', 'feature_pan_mode_1_gridline_4'),
            gridline_5 = TypedString('gridline', 'feature_pan_mode_1_gridline_5'),
            gridline_6 = TypedString('gridline', 'feature_pan_mode_1_gridline_6'),
            gridline_7 = TypedString('gridline', 'feature_pan_mode_1_gridline_7')
          },
          mode_2 = {
            gridline_1 = TypedString('gridline', 'feature_pan_mode_2_gridline_1'),
            gridline_2 = TypedString('gridline', 'feature_pan_mode_2_gridline_2'),
            gridline_3 = TypedString('gridline', 'feature_pan_mode_2_gridline_3'),
            gridline_4 = TypedString('gridline', 'feature_pan_mode_2_gridline_4'),
            gridline_5 = TypedString('gridline', 'feature_pan_mode_2_gridline_5'),
            gridline_6 = TypedString('gridline', 'feature_pan_mode_2_gridline_6'),
            gridline_7 = TypedString('gridline', 'feature_pan_mode_2_gridline_7')
          },
          mode_3 = {
            gridline_1 = TypedString('gridline', 'feature_pan_mode_3_gridline_1'),
            gridline_2 = TypedString('gridline', 'feature_pan_mode_3_gridline_2'),
            gridline_3 = TypedString('gridline', 'feature_pan_mode_3_gridline_3'),
            gridline_4 = TypedString('gridline', 'feature_pan_mode_3_gridline_4'),
            gridline_5 = TypedString('gridline', 'feature_pan_mode_3_gridline_5'),
            gridline_6 = TypedString('gridline', 'feature_pan_mode_3_gridline_6'),
            gridline_7 = TypedString('gridline', 'feature_pan_mode_3_gridline_7')
          },
          mode_4 = {
            gridline_1 = TypedString('gridline', 'feature_pan_mode_4_gridline_1'),
            gridline_2 = TypedString('gridline', 'feature_pan_mode_4_gridline_2'),
            gridline_3 = TypedString('gridline', 'feature_pan_mode_4_gridline_3'),
            gridline_4 = TypedString('gridline', 'feature_pan_mode_4_gridline_4'),
            gridline_5 = TypedString('gridline', 'feature_pan_mode_4_gridline_5'),
            gridline_6 = TypedString('gridline', 'feature_pan_mode_4_gridline_6'),
            gridline_7 = TypedString('gridline', 'feature_pan_mode_4_gridline_7')
          },
          mode_5 = {
            gridline_1 = TypedString('gridline', 'feature_pan_mode_5_gridline_1'),
            gridline_2 = TypedString('gridline', 'feature_pan_mode_5_gridline_2'),
            gridline_3 = TypedString('gridline', 'feature_pan_mode_5_gridline_3'),
            gridline_4 = TypedString('gridline', 'feature_pan_mode_5_gridline_4'),
            gridline_5 = TypedString('gridline', 'feature_pan_mode_5_gridline_5'),
            gridline_6 = TypedString('gridline', 'feature_pan_mode_5_gridline_6'),
            gridline_7 = TypedString('gridline', 'feature_pan_mode_5_gridline_7')
          },
          mode_6 = {
            gridline_1 = TypedString('gridline', 'feature_pan_mode_6_gridline_1'),
            gridline_2 = TypedString('gridline', 'feature_pan_mode_6_gridline_2'),
            gridline_3 = TypedString('gridline', 'feature_pan_mode_6_gridline_3'),
            gridline_4 = TypedString('gridline', 'feature_pan_mode_6_gridline_4'),
            gridline_5 = TypedString('gridline', 'feature_pan_mode_6_gridline_5'),
            gridline_6 = TypedString('gridline', 'feature_pan_mode_6_gridline_6'),
            gridline_7 = TypedString('gridline', 'feature_pan_mode_6_gridline_7')
          },
          mode_7 = {
            gridline_1 = TypedString('gridline', 'feature_pan_mode_7_gridline_1'),
            gridline_2 = TypedString('gridline', 'feature_pan_mode_7_gridline_2'),
            gridline_3 = TypedString('gridline', 'feature_pan_mode_7_gridline_3'),
            gridline_4 = TypedString('gridline', 'feature_pan_mode_7_gridline_4'),
            gridline_5 = TypedString('gridline', 'feature_pan_mode_7_gridline_5'),
            gridline_6 = TypedString('gridline', 'feature_pan_mode_7_gridline_6'),
            gridline_7 = TypedString('gridline', 'feature_pan_mode_7_gridline_7')
          }
        }
      }
    }, overrides or {})
  end

  local function generate_two_dimensional_config(overrides)
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

  it('should require all arguments for a one dimensional config', function()
    should_require_args(grid_deltas, generate_one_dimensional_config())
  end)

  it('should require all arguments for a two dimensional config', function()
    should_require_args(grid_deltas, generate_two_dimensional_config())
  end)

  it('should fail with missing arguments for a one dimensional config', function()
    should_fail_with('missing argument deltas.grid_deltas.mode_1', function()
      local config = generate_one_dimensional_config({
        deltas = { grid_deltas = { mode_2 = {}, mode_3 = {}, mode_4 = {}, mode_5 = {}, mode_6 = {}, mode_7 = {} }}
      })
      grid_deltas(config)
    end)
  end)

  it('should fail with missing arguments for a two dimensional config', function()
    should_fail_with('missing argument deltas.freezer_grid_deltas', function()
      local config = generate_two_dimensional_config({deltas = {fresh_food_grid_deltas = {}}})
      grid_deltas(config)
    end)
  end)

  it('should fail with missing arguments for a two dimensional config', function()
    should_fail_with('missing argument deltas.fresh_food_grid_deltas', function()
      local config = generate_two_dimensional_config({deltas = {freezer_grid_deltas = {}}})
      grid_deltas(config)
    end)
  end)

  it('should generate a typed string with the correct data and type grid_deltas for a one dimensional config', function()
    local expected = remove_whitespace([[
        structure(
          u8(1),
          pointer(
            structure(
              structure(
                u8(7),
                pointer(
                  structure(
                    feature_pan_mode_1_gridline_1,
                    feature_pan_mode_1_gridline_2,
                    feature_pan_mode_1_gridline_3,
                    feature_pan_mode_1_gridline_4,
                    feature_pan_mode_1_gridline_5,
                    feature_pan_mode_1_gridline_6,
                    feature_pan_mode_1_gridline_7,
                    feature_pan_mode_2_gridline_1,
                    feature_pan_mode_2_gridline_2,
                    feature_pan_mode_2_gridline_3,
                    feature_pan_mode_2_gridline_4,
                    feature_pan_mode_2_gridline_5,
                    feature_pan_mode_2_gridline_6,
                    feature_pan_mode_2_gridline_7,
                    feature_pan_mode_3_gridline_1,
                    feature_pan_mode_3_gridline_2,
                    feature_pan_mode_3_gridline_3,
                    feature_pan_mode_3_gridline_4,
                    feature_pan_mode_3_gridline_5,
                    feature_pan_mode_3_gridline_6,
                    feature_pan_mode_3_gridline_7,
                    feature_pan_mode_4_gridline_1,
                    feature_pan_mode_4_gridline_2,
                    feature_pan_mode_4_gridline_3,
                    feature_pan_mode_4_gridline_4,
                    feature_pan_mode_4_gridline_5,
                    feature_pan_mode_4_gridline_6,
                    feature_pan_mode_4_gridline_7,
                    feature_pan_mode_5_gridline_1,
                    feature_pan_mode_5_gridline_2,
                    feature_pan_mode_5_gridline_3,
                    feature_pan_mode_5_gridline_4,
                    feature_pan_mode_5_gridline_5,
                    feature_pan_mode_5_gridline_6,
                    feature_pan_mode_5_gridline_7,
                    feature_pan_mode_6_gridline_1,
                    feature_pan_mode_6_gridline_2,
                    feature_pan_mode_6_gridline_3,
                    feature_pan_mode_6_gridline_4,
                    feature_pan_mode_6_gridline_5,
                    feature_pan_mode_6_gridline_6,
                    feature_pan_mode_6_gridline_7,
                    feature_pan_mode_7_gridline_1,
                    feature_pan_mode_7_gridline_2,
                    feature_pan_mode_7_gridline_3,
                    feature_pan_mode_7_gridline_4,
                    feature_pan_mode_7_gridline_5,
                    feature_pan_mode_7_gridline_6,
                    feature_pan_mode_7_gridline_7
                  )
                )
              )
            )
          )
        )
    ]])

    local actual = grid_deltas(generate_one_dimensional_config())

    assert(actual.is_of_type('grid_deltas'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)

  it('should generate a typed string with the correct data and type grid_deltas for a one dimensional config without all modes defined', function()
    local expected = remove_whitespace([[
        structure(
          u8(1),
          pointer(
            structure(
              structure(
                u8(7),
                pointer(
                  structure(
                    feature_pan_mode_1_gridline_1,
                    feature_pan_mode_1_gridline_2,
                    feature_pan_mode_1_gridline_3,
                    feature_pan_mode_1_gridline_4,
                    feature_pan_mode_1_gridline_5,
                    feature_pan_mode_1_gridline_6,
                    feature_pan_mode_1_gridline_7,
                    feature_pan_mode_2_gridline_1,
                    feature_pan_mode_2_gridline_2,
                    feature_pan_mode_2_gridline_3,
                    feature_pan_mode_2_gridline_4,
                    feature_pan_mode_2_gridline_5,
                    feature_pan_mode_2_gridline_6,
                    feature_pan_mode_2_gridline_7,
                    feature_pan_mode_3_gridline_1,
                    feature_pan_mode_3_gridline_2,
                    feature_pan_mode_3_gridline_3,
                    feature_pan_mode_3_gridline_4,
                    feature_pan_mode_3_gridline_5,
                    feature_pan_mode_3_gridline_6,
                    feature_pan_mode_3_gridline_7,
                    feature_pan_mode_4_gridline_1,
                    feature_pan_mode_4_gridline_2,
                    feature_pan_mode_4_gridline_3,
                    feature_pan_mode_4_gridline_4,
                    feature_pan_mode_4_gridline_5,
                    feature_pan_mode_4_gridline_6,
                    feature_pan_mode_4_gridline_7,
                    feature_pan_mode_5_gridline_1,
                    feature_pan_mode_5_gridline_2,
                    feature_pan_mode_5_gridline_3,
                    feature_pan_mode_5_gridline_4,
                    feature_pan_mode_5_gridline_5,
                    feature_pan_mode_5_gridline_6,
                    feature_pan_mode_5_gridline_7
                  )
                )
              )
            )
          )
        )
    ]])

    local actual = grid_deltas(generate_one_dimensional_config({
      deltas = { grid_deltas = {
        mode_1 = {
          gridline_1 = TypedString('gridline', 'feature_pan_mode_1_gridline_1'),
          gridline_2 = TypedString('gridline', 'feature_pan_mode_1_gridline_2'),
          gridline_3 = TypedString('gridline', 'feature_pan_mode_1_gridline_3'),
          gridline_4 = TypedString('gridline', 'feature_pan_mode_1_gridline_4'),
          gridline_5 = TypedString('gridline', 'feature_pan_mode_1_gridline_5'),
          gridline_6 = TypedString('gridline', 'feature_pan_mode_1_gridline_6'),
          gridline_7 = TypedString('gridline', 'feature_pan_mode_1_gridline_7')
        },
        mode_2 = {
          gridline_1 = TypedString('gridline', 'feature_pan_mode_2_gridline_1'),
          gridline_2 = TypedString('gridline', 'feature_pan_mode_2_gridline_2'),
          gridline_3 = TypedString('gridline', 'feature_pan_mode_2_gridline_3'),
          gridline_4 = TypedString('gridline', 'feature_pan_mode_2_gridline_4'),
          gridline_5 = TypedString('gridline', 'feature_pan_mode_2_gridline_5'),
          gridline_6 = TypedString('gridline', 'feature_pan_mode_2_gridline_6'),
          gridline_7 = TypedString('gridline', 'feature_pan_mode_2_gridline_7')
        },
        mode_3 = {
          gridline_1 = TypedString('gridline', 'feature_pan_mode_3_gridline_1'),
          gridline_2 = TypedString('gridline', 'feature_pan_mode_3_gridline_2'),
          gridline_3 = TypedString('gridline', 'feature_pan_mode_3_gridline_3'),
          gridline_4 = TypedString('gridline', 'feature_pan_mode_3_gridline_4'),
          gridline_5 = TypedString('gridline', 'feature_pan_mode_3_gridline_5'),
          gridline_6 = TypedString('gridline', 'feature_pan_mode_3_gridline_6'),
          gridline_7 = TypedString('gridline', 'feature_pan_mode_3_gridline_7')
        },
        mode_4 = {
          gridline_1 = TypedString('gridline', 'feature_pan_mode_4_gridline_1'),
          gridline_2 = TypedString('gridline', 'feature_pan_mode_4_gridline_2'),
          gridline_3 = TypedString('gridline', 'feature_pan_mode_4_gridline_3'),
          gridline_4 = TypedString('gridline', 'feature_pan_mode_4_gridline_4'),
          gridline_5 = TypedString('gridline', 'feature_pan_mode_4_gridline_5'),
          gridline_6 = TypedString('gridline', 'feature_pan_mode_4_gridline_6'),
          gridline_7 = TypedString('gridline', 'feature_pan_mode_4_gridline_7')
        },
        mode_5 = {
          gridline_1 = TypedString('gridline', 'feature_pan_mode_5_gridline_1'),
          gridline_2 = TypedString('gridline', 'feature_pan_mode_5_gridline_2'),
          gridline_3 = TypedString('gridline', 'feature_pan_mode_5_gridline_3'),
          gridline_4 = TypedString('gridline', 'feature_pan_mode_5_gridline_4'),
          gridline_5 = TypedString('gridline', 'feature_pan_mode_5_gridline_5'),
          gridline_6 = TypedString('gridline', 'feature_pan_mode_5_gridline_6'),
          gridline_7 = TypedString('gridline', 'feature_pan_mode_5_gridline_7')
        }
      }}
    }))

    assert(actual.is_of_type('grid_deltas'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)

  it('should generate a typed string with the correct data and type grid_deltas for a one dimensional config with only one mode defined', function()
    local expected = remove_whitespace([[
        structure(
          u8(1),
          pointer(
            structure(
              structure(
                u8(7),
                pointer(
                  structure(
                    feature_pan_mode_1_gridline_1,
                    feature_pan_mode_1_gridline_2,
                    feature_pan_mode_1_gridline_3,
                    feature_pan_mode_1_gridline_4,
                    feature_pan_mode_1_gridline_5,
                    feature_pan_mode_1_gridline_6,
                    feature_pan_mode_1_gridline_7
                  )
                )
              )
            )
          )
        )
    ]])

    local actual = grid_deltas(generate_one_dimensional_config({
      deltas = { grid_deltas = {
        mode_1 = {
          gridline_1 = TypedString('gridline', 'feature_pan_mode_1_gridline_1'),
          gridline_2 = TypedString('gridline', 'feature_pan_mode_1_gridline_2'),
          gridline_3 = TypedString('gridline', 'feature_pan_mode_1_gridline_3'),
          gridline_4 = TypedString('gridline', 'feature_pan_mode_1_gridline_4'),
          gridline_5 = TypedString('gridline', 'feature_pan_mode_1_gridline_5'),
          gridline_6 = TypedString('gridline', 'feature_pan_mode_1_gridline_6'),
          gridline_7 = TypedString('gridline', 'feature_pan_mode_1_gridline_7')
        }
      }}
    }))

    assert(actual.is_of_type('grid_deltas'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)

  it('should generate a typed string with the correct data and type grid_deltas for a two dimensional config', function()
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

    local actual = grid_deltas(generate_two_dimensional_config())

    assert(actual.is_of_type('grid_deltas'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)

  it('should memoize', function()
    should_memoize_calls(grid_deltas, generate_one_dimensional_config())
  end)

  it('should memoize', function()
    should_memoize_calls(grid_deltas, generate_two_dimensional_config())
  end)
end)

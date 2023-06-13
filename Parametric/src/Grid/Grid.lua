local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local grid_id_type = require 'Grid.GridIdType'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'grid' },
      structure(
        u8(grid_id_type[config.grid_id]),
        pointer(config.grid_deltas),
        u8(config.grid_invalid_freezer_thermistor_fallback_row),
        u8(config.grid_invalid_fresh_food_thermistor_fallback_column),
        i16(config.fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100),
        u16(config.fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf),
        u16(config.grid_periodic_run_rate_in_msec)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        grid_id = { constraint.in_set(enum.keys(grid_id_type)) },
        grid_deltas = { constraint.typed_string('grid_deltas') },
        grid_invalid_freezer_thermistor_fallback_row = { constraint.u8 },
        grid_invalid_fresh_food_thermistor_fallback_column = { constraint.u8 },
        fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = { constraint.i16 },
        fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = { constraint.u16 },
        grid_periodic_run_rate_in_msec = { constraint.u16 }
      })

    return generate(config)
  end
end

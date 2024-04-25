local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local grid_line_correction = require 'Grid.GridLineCorrection'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'gridline' },
      structure(
        i16(config.delta_in_degfx100),
        u8(config.correction),
        pointer(config.cross_ambient_hysteresis_adjustment)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        delta_in_degfx100 = { constraint.i16 },
        correction = { constraint.in_enumeration(grid_line_correction) },
        cross_ambient_hysteresis_adjustment = { constraint.typed_string('cross_ambient_hysteresis_adjustment') }
      })

    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
      return TypedString(
        { 'cross_ambient_offset' },
        structure(
          i16(config.cross_ambient_slope_coefficient),
          i16(config.minimum_cross_ambient_offset_in_degfx100),
          i16(config.maximum_cross_ambient_offset_in_degfx100)
        )
      )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        cross_ambient_slope_coefficient = { constraint.i16 },
        minimum_cross_ambient_offset_in_degfx100 = { constraint.i16 },
        maximum_cross_ambient_offset_in_degfx100 = { constraint.i16 }
      }
    )

    if (config.minimum_cross_ambient_offset_in_degfx100 > config.maximum_cross_ambient_offset_in_degfx100) then
      error('minimum cross ambient offset should be less than maximum cross ambient offset', 2)
    end

    return generate(config)
  end
end

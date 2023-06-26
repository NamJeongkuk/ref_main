local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'convertibleCompartment' },
      structure(
        bool(config.has_convertible_compartment),
        i16(config.cabinet_threshold_in_degfx100)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        has_convertible_compartment = { constraint.boolean },
        cabinet_threshold_in_degfx100 = { constraint.i16 }
      })

    return generate(config)
  end
end
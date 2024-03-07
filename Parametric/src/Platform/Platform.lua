local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local compartment_enum = require 'Platform.compartment_enum'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    local bitmap = 0
    for _, compartment in ipairs(config.compartments) do
    bitmap = bitmap | (1 << compartment_enum[compartment])
    end

    return TypedString(
      { 'platform' },
      structure(
        u8(bitmap),
        u8(config.number_of_evaporators)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        compartments = {
          constraint.array_elements({
            constraint.in_set(enum.keys(compartment_enum))
          })
        },
        number_of_evaporators = { constraint.u8 }
      })

    return generate(config)
  end
end

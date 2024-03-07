local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local interior_light_enum = require 'Lighting.interior_light_enum'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    local lighting_bitmap = 0
    for _, light in ipairs(config.lights) do
      lighting_bitmap = lighting_bitmap | (1 << interior_light_enum[light])
    end

    return TypedString(
      { 'user_allowable_interior_lighting_bitmap' },
      structure(
        u8(lighting_bitmap)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        lights = {
          constraint.array_elements({
            constraint.in_set(enum.keys(interior_light_enum))
          })
        }
      })
    return generate(config)
  end
end

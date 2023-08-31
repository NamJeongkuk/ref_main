local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'user_allowable_interior_lighting' },
      structure(
        config.user_allowable_0_interior_lighting_bitmap,
        config.user_allowable_1_interior_lighting_bitmap,
        config.user_allowable_2_interior_lighting_bitmap
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        user_allowable_0_interior_lighting_bitmap = { constraint.typed_string('user_allowable_interior_lighting_bitmap') },
        user_allowable_1_interior_lighting_bitmap = { constraint.typed_string('user_allowable_interior_lighting_bitmap') },
        user_allowable_2_interior_lighting_bitmap = { constraint.typed_string('user_allowable_interior_lighting_bitmap') },
      })
    return generate(config)
  end
end

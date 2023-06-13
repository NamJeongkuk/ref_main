local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'enhancedSabbath' },
      structure(
        u8(config.number_of_fresh_food_defrosts_before_freezer_defrost)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        number_of_fresh_food_defrosts_before_freezer_defrost = { constraint.u8 }
      })

    return generate(config)
  end
end

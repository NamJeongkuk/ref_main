local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'convertible_damper_step_data' },
      structure(
        config.damper_steps_as_fresh_food,
        config.damper_steps_as_freezer
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        damper_steps_as_fresh_food = { constraint.typed_string('damper_steps') },
        damper_steps_as_freezer = { constraint.typed_string('damper_steps') }
      })
    return generate(config)
  end
end

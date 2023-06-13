local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'recess_heater' },
      structure(
        config.variable_sweat_heater
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        variable_sweat_heater = { constraint.typed_string('variable_sweat_heater') }
      })
    return generate(config)
  end
end

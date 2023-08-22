local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'load_off_door_open_compartment_list' },
      structure(
        config.freezer_compartment
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        freezer_compartment = { constraint.typed_string('load_off_door_open') }
      })
    return generate(config)
  end
end

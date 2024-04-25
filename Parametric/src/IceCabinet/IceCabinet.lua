local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'ice_cabinet' },
      structure(
        pointer(config.gamma_table)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        gamma_table = { constraint.typed_string('gamma_table') },
      })
    return generate(config)
  end
end

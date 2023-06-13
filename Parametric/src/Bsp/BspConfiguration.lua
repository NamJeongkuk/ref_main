local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'bsp_configuration' },
      structure(
        structure(config.binary_search_configuration),
        structure(config.linear_search_configuration)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        binary_search_configuration = { constraint.typed_string('binary_search_configuration') },
        linear_search_configuration = { constraint.typed_string('linear_search_configuration') },
      })
    return generate(config)
  end
end

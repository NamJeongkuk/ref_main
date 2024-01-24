local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local platform_type = require 'Platform.PlatformType'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'platform' },
      structure(
        u8(platform_type[config.platform_type])
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        platform_type = { constraint.in_set(enum.keys(platform_type)) }
      })

    return generate(config)
  end
end

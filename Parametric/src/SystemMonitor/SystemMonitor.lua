
local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'system_monitor' },
      structure(
        u8(config.periodic_nv_update_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        periodic_nv_update_in_minutes = { constraint.u8 }
      })

    return generate(config)
  end
end

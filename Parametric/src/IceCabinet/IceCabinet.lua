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
        pointer(config.gamma_table),
        pointer(config.ice_cabinet_fan_startup),
        pointer(config.freezer_setpoint_to_ice_cabinet_setpoint_table)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        gamma_table = { constraint.typed_string('gamma_table') },
        ice_cabinet_fan_startup = { constraint.typed_string('ice_cabinet_fan_startup') },
        freezer_setpoint_to_ice_cabinet_setpoint_table = { constraint.typed_string('freezer_setpoint_to_ice_cabinet_setpoint_table') }
      }
    )
    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
  import(core)

  local generate = memoize(function(config)
   return TypedString(
     { 'turbo_mode_setpoint' },
     structure(
       pointer(config.fresh_food_turbo_cool_setpoint),
       pointer(config.freezer_turbo_freeze_setpoint)
     )
   )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        fresh_food_turbo_cool_setpoint = { constraint.typed_string('fresh_food_turbo_cool_setpoint') },
        freezer_turbo_freeze_setpoint = { constraint.typed_string('freezer_turbo_freeze_setpoint') }
      })
    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'damper_heater' },
      structure(
        bool(config.damper_heater_on_when_defrost_heater_is_on),
        u8(config.damper_heater_defrost_on_time_in_minutes)
      )
    )
  end)

  return function(config)
    if config.damper_heater_on_when_defrost_heater_is_on == true then
      validate_arguments(
        config,
        {
          damper_heater_on_when_defrost_heater_is_on = { constraint.boolean },
          damper_heater_defrost_on_time_in_minutes = { constraint.equal_to(0) }
        }
      )
    else
      validate_arguments(
        config,
        {
          damper_heater_on_when_defrost_heater_is_on = { constraint.boolean },
          damper_heater_defrost_on_time_in_minutes = { constraint.u8 }
        }
      )
    end
    return generate(config)
  end
end

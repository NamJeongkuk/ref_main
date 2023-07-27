local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    if config.enable == true then
      return TypedString(
        { 'load_off_door_open' },
        structure(
          bool(config.enable),
          u8(config.timeout_in_seconds),
          u8(config.restart_delay_in_seconds)
        )
      )
    else
      return TypedString(
        { 'load_off_door_open' },
        structure(
          bool(config.enable),
          u8(0), -- unused
          u8(0) -- unused
        )
      )
    end
  end)

  return function(config)
    if config.enable == true then
      validate_arguments(
        config,
        {
          enable = { constraint.boolean },
          timeout_in_seconds = { constraint.u8 },
          restart_delay_in_seconds = { constraint.u8 }
        }
      )
    else
      validate_arguments(
        config,
        {
          enable = { constraint.boolean }
        }
      )
    end
    return generate(config)
  end
end

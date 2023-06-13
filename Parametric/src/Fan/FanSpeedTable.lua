local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local Rpm = require 'Fan/Rpm'

return function(core)
  import(core)
  local rpm = Rpm(core)
  local generate = memoize(function(config)
    if config.single_speed_option == nil or config.single_speed_option == false then
      return TypedString(
        { 'fan_speed_table' },
        structure(
          config.super_low_speed,
          config.low_speed,
          config.medium_speed,
          config.high_speed,
          config.super_high_speed
        )
      )
    else
      return TypedString(
          { 'fan_speed_table' },
          structure(
            bool(false),
            config.on_speed
          )
        )
    end
  end)

  return function(config)
    if config.single_speed_option ==  nil  or  config.single_speed_option == false then
      validate_arguments(
        config,
        {
          super_low_speed = { constraint.typed_string('speed') },
          low_speed = { constraint.typed_string('speed') },
          medium_speed = { constraint.typed_string('speed') },
          high_speed = { constraint.typed_string('speed') },
          super_high_speed = { constraint.typed_string('speed') }
        })
    else
      validate_arguments(
        config,
        {
          single_speed_option = { constraint.boolean },
          on_speed = { constraint.typed_string('speed') },
        })
    end
    return generate(config)
  end
end

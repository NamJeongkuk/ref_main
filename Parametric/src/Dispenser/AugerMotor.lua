local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'auger_motor' },
      structure(
        u16(config.auger_motor_reverse_direction_delay_msec),
        u16(config.auger_motor_dispense_complete_msec),
        bool(config.auger_motor_controlled_by_mainboard)
      ))
  end)

  return function(config)
    validate_arguments(
      config,
      {
        auger_motor_reverse_direction_delay_msec = { constraint.u16 },
        auger_motor_dispense_complete_msec = { constraint.u16 },
        auger_motor_controlled_by_mainboard = { constraint.boolean }
      })
    return generate(config)
  end
end

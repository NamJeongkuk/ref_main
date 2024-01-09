local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'dispenser' },
      structure(
        u16(config.maximum_dispensed_ouncesx100),
        u16(config.maximum_dispense_time_in_seconds),
        pointer(config.door_inhibit_water_dispense_table),
        pointer(config.door_inhibit_ice_dispense_table),
        pointer(config.auger_motor)
      ))
  end)

  return function(config)
    validate_arguments(
      config,
      {
        maximum_dispensed_ouncesx100 = { constraint.u16 },
        maximum_dispense_time_in_seconds = { constraint.u16 },
        door_inhibit_water_dispense_table = { constraint.typed_string('door_inhibit_dispense_table') },
        door_inhibit_ice_dispense_table = { constraint.typed_string('door_inhibit_dispense_table') },
        auger_motor = { constraint.typed_string('auger_motor') }
      })
    return generate(config)
  end
end

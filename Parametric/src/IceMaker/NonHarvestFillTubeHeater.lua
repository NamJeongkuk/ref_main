local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'non_harvest_fill_tube_heater' },
      structure(
        i16(config.maximum_cabinet_temperature_in_deg_fx100),
        u8(config.formation_fill_tube_heater_duty_cycle_percentage),
        u8(config.maintenance_fill_tube_heater_duty_cycle_percentage),
        u8(config.off_fill_tube_heater_duty_cycle_percentage)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        maximum_cabinet_temperature_in_deg_fx100 = { constraint.i16 },
        formation_fill_tube_heater_duty_cycle_percentage = { constraint.in_range(0,100) },
        maintenance_fill_tube_heater_duty_cycle_percentage = {  constraint.in_range(0,100) },
        off_fill_tube_heater_duty_cycle_percentage = {  constraint.in_range(0,100) }
      }
    )

    return generate(config)
  end
end

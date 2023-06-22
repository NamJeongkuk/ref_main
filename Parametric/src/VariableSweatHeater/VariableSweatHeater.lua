local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    local heater_control_type do
      for i = 1, 7 do
        if config.variable_sweat_heater.is_of_type('variable_sweat_heater_type_' .. i) then
          heater_control_type = i
          break
        end
      end
    end

    return TypedString(
      { 'variable_sweat_heater' },
      structure(
        u8(heater_control_type),
        u8(config.fallback_duty_cycle_in_percent),
        pointer(config.variable_sweat_heater)
      )
    )
  end)

  return function(config)
    validate_arguments(config, {
      fallback_duty_cycle_in_percent = { constraint.in_range(0, 100) },
      variable_sweat_heater = {
        constraint.ored({
          constraint.typed_string('variable_sweat_heater_type_1'),
          constraint.typed_string('variable_sweat_heater_type_2'),
          constraint.typed_string('variable_sweat_heater_type_3'),
          constraint.typed_string('variable_sweat_heater_type_4'),
          constraint.typed_string('variable_sweat_heater_type_5'),
          constraint.typed_string('variable_sweat_heater_type_6'),
          constraint.typed_string('variable_sweat_heater_type_7')
        })
      }
    })

    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
    import(core)
    local generate = memoize(function(config)
      return TypedString(
        { 'water_valve_flow_rate' },
        structure(
          u8(config.icemaker0_valve_flow_rate_ounces_per_sec_x100),
          u8(config.icemaker1_valve_flow_rate_ounces_per_sec_x100),
          u8(config.icemaker2_valve_flow_rate_ounces_per_sec_x100),
          u8(config.dispenser_valve_flow_rate_ounces_per_sec_x100)
        )
      )
    end)

    return function(config)
      validate_arguments(
        config,
        {
          icemaker0_valve_flow_rate_ounces_per_sec_x100 = { constraint.u8 },
          icemaker1_valve_flow_rate_ounces_per_sec_x100 = { constraint.u8 },
          icemaker2_valve_flow_rate_ounces_per_sec_x100 = { constraint.u8 },
          dispenser_valve_flow_rate_ounces_per_sec_x100 = { constraint.u8 }
        })

      return generate(config)
    end
  end

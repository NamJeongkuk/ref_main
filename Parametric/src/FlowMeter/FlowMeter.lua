local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
    import(core)
    local generate = memoize(function(config)
      return TypedString(
        { 'flow_meter' },
        structure(
          u16(config.flow_meter_oz_per_pulseX100000),
          i8(config.flow_meter_offset_ozX100)
        )
      )
    end)

    return function(config)
      validate_arguments(
        config,
        {
          flow_meter_oz_per_pulseX100000 = { constraint.u16 },
          flow_meter_offset_ozX100 = { constraint.i8 }
        })

      return generate(config)
    end
  end

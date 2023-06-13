local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'ice_maker_fill_monitor' },
      structure(
        u16(config.timed_ice_maker_fill_in_secondsx10),
        u16(config.precise_icemaker_fill_max_time_in_secondsx10),
        u16(config.precise_icemaker_fill_ounces_in_ozx100)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        timed_ice_maker_fill_in_secondsx10 = { constraint.u16 },
        precise_icemaker_fill_max_time_in_secondsx10 = { constraint.greater_than(config.timed_ice_maker_fill_in_secondsx10) },
        precise_icemaker_fill_ounces_in_ozx100 = { constraint.u16 }
      }
    )
    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'common_filter_data' },
      structure(
        u16(config.minimum_volume_needed_to_start_filter_life_timer_in_ounces),
        u16(config.filter_month_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        minimum_volume_needed_to_start_filter_life_timer_in_ounces = { constraint.u16 },
        filter_month_in_minutes = { constraint.u16 }
      }
    )
    return generate(config)
  end
end

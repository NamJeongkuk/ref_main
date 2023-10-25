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
        u32(config.filter_rated_volume_in_ouncesx100),
        u32(config.filter_rated_life_in_minutes),
        u16(config.minimum_volume_needed_to_start_filter_life_timer_in_ounces),
        u32(config.water_filter_calendar_usage_delay_for_low_usage_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        filter_rated_volume_in_ouncesx100 = { constraint.u32 },
        filter_rated_life_in_minutes = { constraint.u32 },
        minimum_volume_needed_to_start_filter_life_timer_in_ounces = { constraint.u16 },
        water_filter_calendar_usage_delay_for_low_usage_in_minutes = { constraint.u32 }
      }
    )
    return generate(config)
  end
end

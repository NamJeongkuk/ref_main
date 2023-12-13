local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
    import(core)
    local generate = memoize(function(config)
      return TypedString(
        { 'ice_maker_fill_blocker' },
        structure(
          u32(config.new_filter_dispenser_purge_volume_before_icemaker_fill_in_ounces_x100),
          u16(config.icemaker_new_filter_fill_state_delay_in_minutes)
        )
      )
    end)

    return function(config)
      validate_arguments(
        config,
        {
          new_filter_dispenser_purge_volume_before_icemaker_fill_in_ounces_x100 = { constraint.u32 },
          icemaker_new_filter_fill_state_delay_in_minutes = { constraint.u16 }
        }
      )
      return generate(config)
    end
  end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'gamma_table' },
      structure(
        u8(config.ice_cabinet_med_speed_limit),
        u8(config.ice_cabinet_high_speed_limit),
        u8(config.ice_cabinet_super_high_speed_limit)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        ice_cabinet_med_speed_limit = { constraint.u8 },
        ice_cabinet_high_speed_limit = { constraint.u8 },
        ice_cabinet_super_high_speed_limit = { constraint.u8 },
      })

    if (config.ice_cabinet_med_speed_limit >= config.ice_cabinet_high_speed_limit
      or config.ice_cabinet_high_speed_limit >= config.ice_cabinet_super_high_speed_limit) then
      error("gamma table limits must be in order and different", 2)
    end

    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'ice_maker' },
      structure(
        pointer(config.freezer_ice_rate),
        pointer(config.aluminum_mold_ice_maker),
        pointer(config.twist_tray_ice_maker),
        pointer(config.non_harvest_fill_tube_heater),
        pointer(config.ice_maker_fill_blocker)
      ))
  end)

  return function(config)
    validate_arguments(
      config,
      {
        freezer_ice_rate = { constraint.typed_string('freezer_ice_rate') },
        aluminum_mold_ice_maker = { constraint.typed_string('aluminum_mold_ice_maker') },
        twist_tray_ice_maker = { constraint.typed_string('twist_tray_ice_maker') },
        non_harvest_fill_tube_heater = { constraint.typed_string('non_harvest_fill_tube_heater') },
        ice_maker_fill_blocker = { constraint.typed_string('ice_maker_fill_blocker')}
      })
    return generate(config)
  end
end

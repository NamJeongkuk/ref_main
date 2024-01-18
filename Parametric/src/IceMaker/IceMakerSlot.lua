local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local ice_maker_type = require 'IceMaker.IceMakerType'
local ice_maker_location = require 'IceMaker.IceMakerLocation'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    if config.enable == true then
      return TypedString(
        { 'ice_maker_slot' },
        structure(
          bool(config.enable),
          u8(config.type),
          u8(config.location),
          pointer(config.ice_maker_data),
          pointer(config.freezer_ice_rate),
          pointer(config.non_harvest_fill_tube_heater)
        )
      )
    else
      return TypedString(
        { 'ice_maker_slot' },
        structure(
          bool(config.enable),
          u8(0),
          u8(0),
          pointer(u8(0)),
          pointer(u8(0)),
          pointer(u8(0))
        )
      )
    end
  end)

  return function(config)
    validate_arguments(
      config,
      {
        enable = { constraint.boolean }
      },
      {
        type = { constraint.in_enumeration(ice_maker_type) },
        location = { constraint.in_enumeration(ice_maker_location) },
        freezer_ice_rate = { constraint.typed_string('freezer_ice_rate') },
        non_harvest_fill_tube_heater = { constraint.typed_string('non_harvest_fill_tube_heater') },
        ice_maker_data = { constraint.ored({
          constraint.typed_string('aluminum_mold_ice_maker'),
          constraint.typed_string('twist_tray_ice_maker')
        })}
      }
    )
    if config.enable == true then
      validate_arguments(
        config,
        {
          enable = { constraint.boolean },
          type = { constraint.in_enumeration(ice_maker_type) },
          location = { constraint.in_enumeration(ice_maker_location) },
          freezer_ice_rate = { constraint.typed_string('freezer_ice_rate') },
          non_harvest_fill_tube_heater = { constraint.typed_string('non_harvest_fill_tube_heater') },
          ice_maker_data = { constraint.ored({
            constraint.typed_string('aluminum_mold_ice_maker'),
            constraint.typed_string('twist_tray_ice_maker')
          })}
        }
      )
      if config.type == ice_maker_type.aluminum_mold then
        validate_arguments(
          config,
          {
            enable = { constraint.boolean },
            type = { constraint.in_enumeration(ice_maker_type) },
            location = { constraint.in_enumeration(ice_maker_location) },
            freezer_ice_rate = { constraint.typed_string('freezer_ice_rate') },
            non_harvest_fill_tube_heater = { constraint.typed_string('non_harvest_fill_tube_heater') },
            ice_maker_data = { constraint.typed_string('aluminum_mold_ice_maker') }
          }
        )
      elseif config.type == ice_maker_type.twist_tray then
        validate_arguments(
          config,
          {
            enable = { constraint.boolean },
            type = { constraint.in_enumeration(ice_maker_type) },
            location = { constraint.in_enumeration(ice_maker_location) },
            freezer_ice_rate = { constraint.typed_string('freezer_ice_rate') },
            non_harvest_fill_tube_heater = { constraint.typed_string('non_harvest_fill_tube_heater') },
            ice_maker_data = { constraint.typed_string('twist_tray_ice_maker') }
          }
        )
      end
    end
    return generate(config)
  end
end

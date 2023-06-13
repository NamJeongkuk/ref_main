local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'fan_list' },
      structure(
        config.condenser_fan,
        config.convertible_compartment_fan,
        config.ice_cabinet_fan,
        config.deli_fan,
        config.freezer_evap_fan,
        config.fresh_food_evap_fan
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        condenser_fan = { constraint.typed_string('condenser_fan') },
        convertible_compartment_fan = { constraint.typed_string('fan') },
        ice_cabinet_fan = { constraint.typed_string('fan') },
        deli_fan = { constraint.typed_string('fan') },
        freezer_evap_fan = { constraint.typed_string('fan') },
        fresh_food_evap_fan = { constraint.typed_string('fan') }
      })
    return generate(config)
  end
end

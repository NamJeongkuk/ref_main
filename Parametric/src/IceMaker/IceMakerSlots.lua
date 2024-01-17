local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'ice_maker_slots' },
      structure(
        pointer(config.ice_maker_slot_0),
        pointer(config.ice_maker_slot_1),
        pointer(config.ice_maker_slot_2)
      ))
  end)

  return function(config)
   validate_arguments(
     config,
     {
       ice_maker_slot_0 = { constraint.typed_string('ice_maker_slot') },
       ice_maker_slot_1 = { constraint.typed_string('ice_maker_slot') },
       ice_maker_slot_2 = { constraint.typed_string('ice_maker_slot') }
     })
   return generate(config)
 end
end

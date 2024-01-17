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
        pointer(config.ice_maker_slots),
        pointer(config.ice_maker_fill_blocker)
      ))
  end)

  return function(config)
    validate_arguments(
      config,
      {
        ice_maker_slots = { constraint.typed_string('ice_maker_slots') },
        ice_maker_fill_blocker = { constraint.typed_string('ice_maker_fill_blocker')}
      })
    return generate(config)
  end
end

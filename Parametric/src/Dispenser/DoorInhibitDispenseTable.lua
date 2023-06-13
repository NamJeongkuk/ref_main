local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'door_inhibit_dispense_table' },
      structure(
        bool(config.left_hand_fresh_food),
        bool(config.right_hand_fresh_food),
        bool(config.left_hand_freezer),
        bool(config.right_hand_freezer),
        bool(config.convertible_compartment),
        bool(config.door_in_door)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        left_hand_fresh_food = { constraint.boolean },
        right_hand_fresh_food = { constraint.boolean },
        left_hand_freezer = { constraint.boolean },
        right_hand_freezer = { constraint.boolean },
        convertible_compartment = { constraint.boolean },
        door_in_door = { constraint.boolean }
      })

    return generate(config)
  end
end

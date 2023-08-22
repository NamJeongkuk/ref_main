local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local doors_enum = require 'Cooling.doors_enum'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    if config.enable == true then
      local door_bitmap = 0
      for _, door in ipairs(config.doors) do
        door_bitmap = door_bitmap | (1 << doors_enum[door])
      end

      return TypedString(
        { 'load_off_door_open' },
        structure(
          bool(config.enable),
          u8(door_bitmap),
          u8(config.timeout_in_seconds),
          u8(config.restart_delay_in_seconds)
        )
      )
    else
      return TypedString(
        { 'load_off_door_open' },
        structure(
          bool(config.enable),
          u8(0), -- unused
          u8(0), -- unused
          u8(0) -- unused
        )
      )
    end
  end)

  return function(config)
    if config.enable == true then
      validate_arguments(
        config,
        {
          enable = { constraint.boolean },
          doors = {
            constraint.array_size({ constraint.greater_than(0) }),
            constraint.array_elements({
              constraint.in_set(enum.keys(doors_enum))
            })
          },
          timeout_in_seconds = { constraint.u8 },
          restart_delay_in_seconds = { constraint.u8 }
        }
      )
    else
      validate_arguments(
        config,
        {
          enable = { constraint.boolean }
        }
      )
    end
    return generate(config)
  end
end

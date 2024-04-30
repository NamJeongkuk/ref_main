local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local map = require 'lua-common'.fp.map

return function(core)
  import(core)

  local unpack_table = function(table)
    return structure(i16(table.freezer), i16(table.ice_cabinet))
  end

  local generate = memoize(function(config)
    return TypedString(
      { 'freezer_setpoint_to_ice_cabinet_setpoint_table' },
      structure(
        pointer(
          structure(
            table.unpack(map(config.data, unpack_table))
          )
        ),
        u8(#config.data)
      )
    )
  end)

  return function(config)
    validate_arguments(config, {
      data = {
        constraint.array_size({ constraint.positive }),
        constraint.array_elements({
          constraint.table_keys({
            freezer = { constraint.in_range(-32768, 32767) },
            ice_cabinet = { constraint.in_range(-32768, 32767) }
          })
        })
      }
    })

    for i = 1, #config.data - 1 do
      if config.data[i].freezer > config.data[i+1].freezer then
        error('freezer setpoints must be in ascending order', 2)
      end
    end

    return generate(config)
  end
end

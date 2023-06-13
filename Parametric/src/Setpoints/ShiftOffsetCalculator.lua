local import = require 'lua-common'.utilities.import
local memoize = require 'lua-common'.util.memoize
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString

return function(core)
   import(core)

   local generate = memoize(function(config)
    return TypedString(
      { 'shift_offset_calculator' },
      structure(
        u8(config.long_term_beta),
        u8(config.long_term_average_update_time_in_minutes),
        u8(config.shift_update_time_in_minutes)
      )
    )
   end)

  return function(config)
    validate_arguments(
      config,
      {
        long_term_beta = { constraint.u8 },
        long_term_average_update_time_in_minutes = { constraint.u8, constraint.less_than(config.shift_update_time_in_minutes) },
        shift_update_time_in_minutes = { constraint.u8 }
      })
    return generate(config)
  end
end

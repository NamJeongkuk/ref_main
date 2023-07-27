local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
   import(core)
   local generate = memoize(function(config)
     return TypedString(
       { 'lighting' },
       structure(
         u8(config.maximum_light_duty_cycle_percentage),
         pointer(config.fresh_food_backwall),
         pointer(config.fresh_food_top_and_side),
         pointer(config.freezer_backwall),
         pointer(config.freezer_top_and_side)
       )
     )
   end)

   return function(config)
     validate_arguments(
       config,
       {
         maximum_light_duty_cycle_percentage = { constraint.in_range(0,100) },
         fresh_food_backwall = { constraint.typed_string('door_lighting') },
         fresh_food_top_and_side = { constraint.typed_string('door_lighting') },
         freezer_backwall = { constraint.typed_string('door_lighting') },
         freezer_top_and_side = { constraint.typed_string('door_lighting')}
       })
     return generate(config)
   end
 end

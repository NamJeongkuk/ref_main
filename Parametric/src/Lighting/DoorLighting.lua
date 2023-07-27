local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
   import(core)
   local generate = memoize(function(config)
     return TypedString(
       { 'door_lighting' },
       structure(
         pointer(config.normal_operation_ramping_pwm_duty_cycle_percentage_bundle),
         pointer(config.normal_sabbath_ramping_pwm_duty_cycle_percentage_bundle),
         pointer(config.enhanced_sabbath_ramping_pwm_duty_cycle_percentage_bundle),
         pointer(config.snack_mode_ramping_pwm_duty_cycle_percentage_bundle)
       )
     )
   end)

   return function(config)
     validate_arguments(
       config,
       {
         normal_operation_ramping_pwm_duty_cycle_percentage_bundle = { constraint.typed_string('ramping_pwm_duty_cycle_percentage_bundle') },
         normal_sabbath_ramping_pwm_duty_cycle_percentage_bundle = { constraint.typed_string('ramping_pwm_duty_cycle_percentage_bundle') },
         enhanced_sabbath_ramping_pwm_duty_cycle_percentage_bundle = { constraint.typed_string('ramping_pwm_duty_cycle_percentage_bundle') },
         snack_mode_ramping_pwm_duty_cycle_percentage_bundle = { constraint.typed_string('ramping_pwm_duty_cycle_percentage_bundle') }
       })
     return generate(config)
   end
 end

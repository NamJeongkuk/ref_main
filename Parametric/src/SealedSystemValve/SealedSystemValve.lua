local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local enum = require 'lua-common'.utilities.enum
local sealed_system_valve_position_type = require 'SealedSystemValve.SealedSystemValvePositionType'

local function error_if_any_position_is_the_same(A, B, C, D)
  if A==B or A==C or A==D or B==C or B==D or C==D then
    error('All of the sealed system step positions must be unique')
  end
end

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'sealed_system_valve' },
      structure(
        u8(config.step_position_A),
        u8(config.step_position_B),
        u8(config.step_position_C),
        u8(config.step_position_D),
        u16(config.homing_steps),
        u16(config.coil_excitation_delay_in_ms),
        u8(config.delay_between_steps_in_ms),
        u8(sealed_system_valve_position_type[config.power_up_service_position]),
        u8(config.power_up_time_in_seconds)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        step_position_A = { constraint.u8 },
        step_position_B = { constraint.u8 },
        step_position_C = { constraint.u8 },
        step_position_D = { constraint.u8 },
        homing_steps = { constraint.u16 },
        coil_excitation_delay_in_ms = { constraint.u16 },
        delay_between_steps_in_ms = { constraint.u8 },
        power_up_service_position = { constraint.in_set(enum.keys(sealed_system_valve_position_type)) },
        power_up_time_in_seconds = { constraint.u8 }
      })

    error_if_any_position_is_the_same(
      config.step_position_A,
      config.step_position_B,
      config.step_position_C,
      config.step_position_D)

    return generate(config)
  end
end

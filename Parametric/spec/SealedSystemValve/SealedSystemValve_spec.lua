local SealedSystemValve = require 'SealedSystemValve/SealedSystemValve'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local sealed_system_valve_position_type = require 'SealedSystemValve.SealedSystemValvePositionType'

describe('SealedSystemValve', function()
  local sealed_system_valve = SealedSystemValve(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      step_position_A = 200,
      step_position_B = 100,
      step_position_C = 50,
      step_position_D = 30,
      homing_steps = 200,
      coil_excitation_delay_in_ms = 500,
      delay_between_steps_in_ms = 33,
      power_up_service_position = 'position_A',
      power_up_time_in_seconds = 5
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(sealed_system_valve, generate_config())
  end)

  it('should assert if homing_steps is not in range', function()
    should_fail_with('homing_steps=-1 must be in [0, 65535]', function()
      sealed_system_valve(generate_config({
        homing_steps = -1
      }))
    end)
  end)

  it('should assert if delay_between_steps_in_ms is not in range', function()
    should_fail_with('delay_between_steps_in_ms=-1 must be in [0, 255]', function()
      sealed_system_valve(generate_config({
        delay_between_steps_in_ms = -1
      }))
    end)
  end)

  it('should assert if coil_excitation_delay_in_ms is not in range', function()
    should_fail_with('coil_excitation_delay_in_ms=-1 must be in [0, 65535]', function()
      sealed_system_valve(generate_config({
        coil_excitation_delay_in_ms = -1
      }))
    end)
  end)

  it('should assert if step_position_A is not in range', function()
    should_fail_with('step_position_A=-1 must be in [0, 255]', function()
      sealed_system_valve(generate_config({
        step_position_A = -1
      }))
    end)
  end)

  it('should assert if step_position_B is not in range', function()
    should_fail_with('step_position_B=-1 must be in [0, 255]', function()
      sealed_system_valve(generate_config({
        step_position_B = -1
      }))
    end)
  end)

  it('should assert if step_position_C is not in range', function()
    should_fail_with('step_position_C=-1 must be in [0, 255]', function()
      sealed_system_valve(generate_config({
        step_position_C = -1
      }))
    end)
  end)

  it('should assert if step_position_D is not in range', function()
    should_fail_with('step_position_D=-1 must be in [0, 255]', function()
      sealed_system_valve(generate_config({
        step_position_D = -1
      }))
    end)
  end)

  it('should assert if step_position_A is equal to step_position_B', function()
    should_fail_with('All of the sealed system step positions must be unique', function()
      sealed_system_valve(generate_config({
        step_position_A = 1,
        step_position_B = 1
      }))
    end)
  end)

  it('should assert if step_position_A is equal to step_position_C', function()
    should_fail_with('All of the sealed system step positions must be unique', function()
      sealed_system_valve(generate_config({
        step_position_A = 1,
        step_position_C = 1
      }))
    end)
  end)

  it('should assert if step_position_A is equal to step_position_D', function()
    should_fail_with('All of the sealed system step positions must be unique', function()
      sealed_system_valve(generate_config({
        step_position_A = 1,
        step_position_D = 1
      }))
    end)
  end)

  it('should assert if step_position_B is equal to step_position_C', function()
    should_fail_with('All of the sealed system step positions must be unique', function()
      sealed_system_valve(generate_config({
        step_position_B = 1,
        step_position_C = 1
      }))
    end)
  end)

  it('should assert if step_position_B is equal to step_position_D', function()
    should_fail_with('All of the sealed system step positions must be unique', function()
      sealed_system_valve(generate_config({
        step_position_B = 1,
        step_position_D = 1
      }))
    end)
  end)

  it('should assert if step_position_C is equal to step_position_D', function()
    should_fail_with('All of the sealed system step positions must be unique', function()
      sealed_system_valve(generate_config({
        step_position_C = 1,
        step_position_D = 1
      }))
    end)
  end)

  it('should require sealed system valve position argument of power_up_service_position to be valid sealed system valve position', function()
    should_fail_with("power_up_service_position='not a valid position' must be in the set { 'position_home', 'position_A', 'position_B', 'position_C', 'position_D' }", function()
      sealed_system_valve(generate_config({
        power_up_service_position = 'not a valid position'
      }))
    end)
  end)

  it('should assert if power_up_time_in_seconds is not in range', function()
    should_fail_with('power_up_time_in_seconds=-1 must be in [0, 255]', function()
      sealed_system_valve(generate_config({
        power_up_time_in_seconds = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type sealed_system_valve', function()
    local expected = remove_whitespace([[
    structure(
      u8(200),
      u8(100),
      u8(50),
      u8(30),
      u16(200),
      u16(500),
      u8(33),
      u8(]] .. sealed_system_valve_position_type.position_A .. [[),
      u8(5))
    ]])

    local actual = sealed_system_valve({
      step_position_A = 200,
      step_position_B = 100,
      step_position_C = 50,
      step_position_D = 30,
      homing_steps = 200,
      coil_excitation_delay_in_ms = 500,
      delay_between_steps_in_ms = 33,
      power_up_service_position = 'position_A',
      power_up_time_in_seconds = 5
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('sealed_system_valve'))
  end)

  it('should memoize', function()
    should_memoize_calls(sealed_system_valve, generate_config())
  end)

end)

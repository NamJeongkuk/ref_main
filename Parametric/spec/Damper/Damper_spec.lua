local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local Damper = require 'Damper/Damper'
local ConvertibleDamperStepData = require 'Damper/ConvertibleDamperStepData'
local DamperSteps = require 'Damper/DamperSteps'
local damper_id = require 'Damper/DamperId'

describe('Damper', function()
  local damper = Damper(core_mock)
  local damper_steps = DamperSteps(core_mock)
  local convertible_damper_step_data = ConvertibleDamperStepData(core_mock)

  local damper_steps_as_fresh_food = damper_steps({
    steps_to_open = 650,
    steps_to_close = 700
  })

  local damper_steps_as_freezer = damper_steps({
    steps_to_open = 651,
    steps_to_close = 701
  })

  local convertible_damper_steps = convertible_damper_step_data({
    damper_steps_as_fresh_food = damper_steps_as_fresh_food,
    damper_steps_as_freezer = damper_steps_as_freezer
  })

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      damper_id = damper_id.normal_damper,
      damper_step_data = TypedString('damper_steps', 'damper steps data'),
      steps_to_home = 1850,
      delay_between_step_events_in_hundreds_of_microseconds = 1,
      max_time_for_damper_to_be_open_before_cycling_in_minutes = 11,
      target_compartment_minimum_temperature_change_time_in_minutes = 3,
      target_compartment_damper_heater_on_time_in_minutes = 2,
      source_compartment_maximum_temperature_to_run_check_in_degfx100 = 500,
      target_compartment_minimum_temperature_change_in_degfx100 = 10
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(damper, generate_config())
  end)

  it('should assert if damper_id is not a damper_id', function()
    should_fail_with('damper_id=totally_not_a_damper is not in the damper_id enumeration, allowable values: convertible_compartment_damper, normal_damper', function()
      damper(generate_config({
        damper_id = 'totally_not_a_damper'
      }))
    end)
  end)

  it('should assert if damper_step_data is not correct type for damper_id', function()
    should_fail_with('damper_step_data must be a typed string with type damper_steps, has type(s) blah', function()
      damper(generate_config({
        damper_id = damper_id.normal_damper,
        damper_step_data = TypedString('blah', 'some_blah')
      }))
    end)

    should_fail_with('damper_step_data must be a typed string with type convertible_damper_step_data, has type(s) blah', function()
      damper(generate_config({
        damper_id = damper_id.convertible_compartment_damper,
        damper_step_data = TypedString('blah', 'some_blah')
      }))
    end)
  end)

  it('should assert if steps_to_home is not in range', function()
    should_fail_with('steps_to_home=65536 must be in [0, 65535]', function()
      damper(generate_config({
        steps_to_home = 65536
      }))
    end)
  end)

  it('should assert if delay_between_step_events_in_hundreds_of_microseconds is not in range', function()
    should_fail_with('delay_between_step_events_in_hundreds_of_microseconds=256 must be in [0, 255]', function()
      damper(generate_config({
        delay_between_step_events_in_hundreds_of_microseconds = 256
      }))
    end)
  end)

  it('should assert if max_time_for_damper_to_be_open_before_cycling_in_minutes is not in range', function()
    should_fail_with('max_time_for_damper_to_be_open_before_cycling_in_minutes=256 must be in [0, 255]', function()
      damper(generate_config({
        max_time_for_damper_to_be_open_before_cycling_in_minutes = 256
      }))
    end)
  end)

  it('should assert if target_compartment_minimum_temperature_change_time_in_minutes is not in range', function()
    should_fail_with('target_compartment_minimum_temperature_change_time_in_minutes=256 must be in [0, 255]', function()
      damper(generate_config({
        target_compartment_minimum_temperature_change_time_in_minutes = 256
      }))
    end)
  end)

  it('should assert if target_compartment_damper_heater_on_time_in_minutes is not in range', function()
    should_fail_with('target_compartment_damper_heater_on_time_in_minutes=256 must be in [0, 255]', function()
      damper(generate_config({
        target_compartment_damper_heater_on_time_in_minutes = 256
      }))
    end)
  end)

  it('should assert if source_compartment_maximum_temperature_to_run_check_in_degfx100 is not in range', function()
    should_fail_with('source_compartment_maximum_temperature_to_run_check_in_degfx100=-100000 must be in [-32768, 32767]', function()
      damper(generate_config({
        source_compartment_maximum_temperature_to_run_check_in_degfx100 = -100000
      }))
    end)
  end)

  it('should assert if target_compartment_minimum_temperature_change_in_degfx100 is not in range', function()
    should_fail_with('target_compartment_minimum_temperature_change_in_degfx100=-100000 must be in [-32768, 32767]', function()
      damper(generate_config({
        target_compartment_minimum_temperature_change_in_degfx100 = -100000
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for single damper', function()
    local expected = remove_whitespace([[
      structure(
        u8(0),
        pointer(
          structure(
            u16(650),
            u16(700)
          )
        ),
        u16(1850),
        u8(1),
        u8(11),
        u8(3),
        u8(2),
        i16(500),
        i16(10)
      )
    ]])

    local actual = damper({
      damper_id = damper_id.normal_damper,
      damper_step_data = damper_steps_as_fresh_food,
      steps_to_home = 1850,
      delay_between_step_events_in_hundreds_of_microseconds = 1,
      max_time_for_damper_to_be_open_before_cycling_in_minutes = 11,
      target_compartment_minimum_temperature_change_time_in_minutes = 3,
      target_compartment_damper_heater_on_time_in_minutes = 2,
      source_compartment_maximum_temperature_to_run_check_in_degfx100 = 500,
      target_compartment_minimum_temperature_change_in_degfx100 = 10
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('damper'))
  end)

  it('should generate a typed string with the correct data and type for convertible compartment damper', function()
    local expected = remove_whitespace([[
      structure(
        u8(1),
        pointer(
          structure(
            structure(
              u16(650),
              u16(700)
            ),
            structure(
              u16(651),
              u16(701)
            )
          )
        ),
        u16(1850),
        u8(1),
        u8(11),
        u8(3),
        u8(2),
        i16(500),
        i16(10)
      )
    ]])

    local actual = damper({
      damper_id = damper_id.convertible_compartment_damper,
      damper_step_data = convertible_damper_steps,
      steps_to_home = 1850,
      delay_between_step_events_in_hundreds_of_microseconds = 1,
      max_time_for_damper_to_be_open_before_cycling_in_minutes = 11,
      target_compartment_minimum_temperature_change_time_in_minutes = 3,
      target_compartment_damper_heater_on_time_in_minutes = 2,
      source_compartment_maximum_temperature_to_run_check_in_degfx100 = 500,
      target_compartment_minimum_temperature_change_in_degfx100 = 10
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('damper'))
  end)

  it('should memoize', function()
    should_memoize_calls(damper, generate_config())
  end)
end)

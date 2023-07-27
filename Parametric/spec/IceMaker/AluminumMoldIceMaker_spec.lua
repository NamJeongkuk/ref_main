local AluminumMoldIceMaker = require 'IceMaker/AluminumMoldIceMaker'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local clone = require 'lua-common'.utilities.clone
local TypedString = require 'lua-common'.util.TypedString
local ice_maker_type = require 'IceMaker/IceMakerType'
local ice_maker_location = require 'IceMaker/IceMakerLocation'

describe('AluminumMoldIceMaker', function()
  local aluminum_mold_ice_maker = AluminumMoldIceMaker(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.deep_merge({
      information = {
        type = 'aluminum_mold',
        location = 'freezer'
      },
      fill = {
        ice_maker_fill_monitor = TypedString('ice_maker_fill_monitor', 'ice_maker_fill_monitor')
      },
      freeze = {
        harvest_count_calculator = TypedString('harvest_count_calculator', 'harvest_count_calculator'),
        maximum_harvest_temperature_in_deg_fx100 = 1900,
        minimum_feeler_arm_extension_time_in_minutes = 3
      },
      harvest = {
        maximum_harvest_time_in_minutes = 7,
        initial_motor_on_temperature_in_deg_fx100 = 3560,
        initial_minimum_heater_on_time_in_seconds = 30,
        heater_off_temperature_in_deg_fx100 = 5000,
        heater_on_temperature_in_deg_fx100 = 3200,
        rake_not_home_test_time_in_seconds = 10,
        feeler_arm_test_time_in_seconds = 10,
        freeze_thaw_fill_tube_heater_on_time_in_seconds = 400,
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100
      },
      harvest_fault = {
        rake_motor_control_time_in_seconds = 30,
        harvest_fault_max_time_in_minutes = 360
      },
      harvest_fix = {
        heater_off_temperature_in_deg_fx100 = 7800,
        heater_on_temperature_in_deg_fx100 = 6900,
        motor_off_time_in_seconds = 30,
        motor_on_time_in_seconds = 30,
        maximum_harvest_fix_time_in_minutes = 60
      }
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(aluminum_mold_ice_maker, generate_config())
  end)

  it('should assert if information.type is not valid', function()
    should_fail_with("information.type=-1 must be in the set { 'aluminum_mold', 'twist', 'cartridge', 'nugget' }", function()
      aluminum_mold_ice_maker(generate_config({
        information = {
          type = -1
        }
      }))
    end)
  end)

  it('should assert if infromation.location is not valid', function()
    should_fail_with("information.location=-1 must be in the set { 'freezer', 'fridge' }", function()
      aluminum_mold_ice_maker(generate_config({
        information = {
          location = -1
        }
      }))
    end)
  end)

  it('should assert if fill.ice_maker_fill_monitor is not a string', function()
    should_fail_with('fill.ice_maker_fill_monitor must be a typed string with type ice_maker_fill_monitor, but is a number', function()
      aluminum_mold_ice_maker(generate_config({
        fill = {
          ice_maker_fill_monitor = 1
        }
      }))
    end)
  end)

  it('should assert if freeze.harvest_count_calculator is not a string', function()
    should_fail_with('freeze.harvest_count_calculator must be a typed string with type harvest_count_calculator, but is a number', function()
      aluminum_mold_ice_maker(generate_config({
        freeze = {
          harvest_count_calculator = 1
        }
      }))
    end)
  end)

  it('should assert if maximum_harvest_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('maximum_harvest_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      aluminum_mold_ice_maker(generate_config({
        freeze = {
          maximum_harvest_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if minimum_feeler_arm_extension_time_in_minutes is not in range', function()
    should_fail_with('minimum_feeler_arm_extension_time_in_minutes=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        freeze = {
          minimum_feeler_arm_extension_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if maximum_harvest_time_in_minutes is not in range', function()
    should_fail_with('maximum_harvest_time_in_minutes=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          maximum_harvest_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if initial_motor_on_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('initial_motor_on_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          initial_motor_on_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if initial_minimum_heater_on_time_in_seconds is not in range', function()
    should_fail_with('initial_minimum_heater_on_time_in_seconds=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          initial_minimum_heater_on_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if heater_off_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('heater_off_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          heater_off_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if heater_on_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('heater_on_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          heater_on_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if rake_not_home_test_time_in_seconds is not in range', function()
    should_fail_with('rake_not_home_test_time_in_seconds=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          rake_not_home_test_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if feeler_arm_test_time_in_seconds is not in range', function()
    should_fail_with('feeler_arm_test_time_in_seconds=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          feeler_arm_test_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if rake_motor_control_time_in_seconds is not in range', function()
    should_fail_with('rake_motor_control_time_in_seconds=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest_fault = {
          rake_motor_control_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if harvest_fault_max_time_in_minutes is not in range', function()
    should_fail_with('harvest_fault_max_time_in_minutes=-1 must be in [0, 65535]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest_fault = {
          harvest_fault_max_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if freeze_thaw_fill_tube_heater_duty_cycle_percentage is not in range', function()
    should_fail_with('freeze_thaw_fill_tube_heater_duty_cycle_percentage=101 must be in [0, 100]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          freeze_thaw_fill_tube_heater_duty_cycle_percentage = 101
        }
      }))
    end)
  end)

  it('should assert if heater_off_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('heater_off_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest_fix = {
          heater_off_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if freeze_thaw_fill_tube_heater_on_time_in_seconds is not in range', function()
    should_fail_with('harvest.freeze_thaw_fill_tube_heater_on_time_in_seconds=-1 must be in [0, 65535]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          freeze_thaw_fill_tube_heater_on_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if heater_on_temperature_in_deg_fx100 is not in range', function()
    should_fail_with('heater_on_temperature_in_deg_fx100=32768 must be in [-32768, 32767]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest_fix = {
          heater_on_temperature_in_deg_fx100 = 32768
        }
      }))
    end)
  end)

  it('should assert if motor_off_time_in_seconds is not in range', function()
    should_fail_with('motor_off_time_in_seconds=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest_fix = {
          motor_off_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if motor_on_time_in_seconds is not in range', function()
    should_fail_with('motor_on_time_in_seconds=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest_fix = {
          motor_on_time_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if maximum_harvest_fix_time_in_minutes is not in range', function()
    should_fail_with('maximum_harvest_fix_time_in_minutes=-1 must be in [0, 255]', function()
      aluminum_mold_ice_maker(generate_config({
        harvest_fix = {
          maximum_harvest_fix_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if maximum_harvest_time_in_minutes*seconds_per_minute is less than freeze_thaw_fill_tube_heater_on_time_in_seconds', function()
    should_fail_with('freeze thaw fill tube heater on time must be less than or equal to maximum harvest time', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          maximum_harvest_time_in_minutes = 4,
          freeze_thaw_fill_tube_heater_on_time_in_seconds = 300
        }
      }))
    end)
  end)

  it('should assert if initial_minimum_heater_on_time_in_seconds is greater than freeze_thaw_fill_tube_heater_on_time_in_seconds', function()
    should_fail_with('minimum heater on time must be less than or equal to freeze thaw fill tube heater on time', function()
      aluminum_mold_ice_maker(generate_config({
        harvest = {
          initial_minimum_heater_on_time_in_seconds = 240,
          freeze_thaw_fill_tube_heater_on_time_in_seconds = 20
        }
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for aluminum mold ice maker', function()
    local expected = remove_whitespace([[
        structure(
          structure(
            u8(2),
            u8(1)
          ),
          structure(
            pointer(ice_maker_fill_monitor)
          ),
          structure(
            pointer(harvest_count_calculator),
            i16(1900),
            u8(3)
          ),
          structure(
            u8(7),
            i16(3560),
            u8(30),
            i16(5000),
            i16(3200),
            u8(10),
            u8(10),
            u16(400),
            u8(100)
          ),
          structure(
            u8(30),
            u16(360)
          ),
          structure(
            i16(7800),
            i16(6900),
            u8(30),
            u8(30),
            u8(60)
          )
        )
      ]])

    local actual = aluminum_mold_ice_maker({
      information = {
        type = 'cartridge',
        location = 'fridge'
      },
      fill = {
        ice_maker_fill_monitor = TypedString('ice_maker_fill_monitor', 'ice_maker_fill_monitor')
      },
      freeze = {
        harvest_count_calculator = TypedString('harvest_count_calculator', 'harvest_count_calculator'),
        maximum_harvest_temperature_in_deg_fx100 = 1900,
        minimum_feeler_arm_extension_time_in_minutes = 3
      },
      harvest = {
        maximum_harvest_time_in_minutes = 7,
        initial_motor_on_temperature_in_deg_fx100 = 3560,
        initial_minimum_heater_on_time_in_seconds = 30,
        heater_off_temperature_in_deg_fx100 = 5000,
        heater_on_temperature_in_deg_fx100 = 3200,
        rake_not_home_test_time_in_seconds = 10,
        feeler_arm_test_time_in_seconds = 10,
        freeze_thaw_fill_tube_heater_on_time_in_seconds = 400,
        freeze_thaw_fill_tube_heater_duty_cycle_percentage = 100
      },
      harvest_fault = {
        rake_motor_control_time_in_seconds = 30,
        harvest_fault_max_time_in_minutes = 360
      },
      harvest_fix = {
        heater_off_temperature_in_deg_fx100 = 7800,
        heater_on_temperature_in_deg_fx100 = 6900,
        motor_off_time_in_seconds = 30,
        motor_on_time_in_seconds = 30,
        maximum_harvest_fix_time_in_minutes = 60
      }
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('aluminum_mold_ice_maker'))
  end)

  it('should memoize', function()
    should_memoize_calls(aluminum_mold_ice_maker, generate_config())
  end)
end)

local Defrost = require 'Defrost/Defrost'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local sealed_system_valve_position_type = require 'SealedSystemValve.SealedSystemValvePositionType'
local damper_position_type =  require 'Damper.DamperPositionType'
local TypedString = require 'lua-common'.utilities.TypedString
local fan_speed_type = require 'Fan.FanSpeedType'
local compressor_speed_type = require 'Compressor.CompressorSpeedType'

describe('Defrost', function()
  local defrost = Defrost(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.deep_merge({
      idle = {
        freezer_door_increment_factor_in_seconds_per_second = 348,
        fresh_food_door_increment_factor_in_seconds_per_second = 87,
        minimum_time_between_defrosts_abnormal_run_time_in_minutes = 6 * 60,
        max_time_between_defrosts_in_minutes = 32 * 60,
        aham_prechill_time_between_defrosts_in_minutes = 6 * 60
      },
      prechill_prep = {
        number_of_secondary_only_defrosts_before_full_defrost = 2,
        number_of_secondary_only_defrosts_before_full_defrost_when_abnormal_is_set = 1,
        max_prechill_prep_time_in_minutes = 0
      },
      prechill = {
        max_prechill_time_in_minutes = 10,
        max_prechill_time_for_secondary_only_defrost_in_minutes = 20,
        prechill_sealed_system_valve_position = 'position_B',
        prechill_compressor_speed = 'low_speed',
        prechill_freezer_evap_fan_speed = 'super_low_speed',
        prechill_fresh_food_evap_fan_speed = 'low_speed',
        prechill_ice_cabinet_fan_speed = 'medium_speed',
        prechill_deli_fan_speed = 'high_speed',
        prechill_convertible_compartment_damper_position = 'open',
        prechill_convertible_compartment_fan_speed = 'super_low_speed',
        prechill_fresh_food_damper_position = 'closed',
        prechill_freezer_min_temp_in_degfx100 = -600,
        prechill_fresh_food_min_temp_in_degfx100 = 3200,
        prechill_fresh_food_max_temp_in_degfx100 = 4600,
        prechill_convertible_compartment_as_freezer_min_temp_in_degfx100 = -600,
        prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100 = 3200,
        prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100 = 4600,
        prechill_freezer_evap_exit_temperature_in_degfx100 = -3000,
        prechill_convertible_compartment_evap_exit_temperature_in_degfx100 = -3000
      },
      heater_on_entry = {
        defrost_heater_on_delay_after_compressor_off_in_seconds = 2,
        heater_on_entry_fresh_food_damper_position = 'closed',
        heater_on_entry_sealed_system_valve_position = 'position_A'
      },
      heater_on = {
        freezer_defrost_heater_max_on_time_in_minutes = 60,
        freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 30,
        freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = 32,
        freezer_defrost_termination_temperature_in_degfx100 = 5900,
        secondary_only_defrost_termination_temperature_in_degfx100 = 4460,
        convertible_compartment_defrost_termination_temperature_in_degfx100 = 4460,
        fresh_food_defrost_heater_max_on_time_in_minutes = 60,
        fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 20,
        fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = 21,
        convertible_compartment_defrost_heater_max_on_time_in_minutes = 60,
        convertible_compartment_as_fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = 21,
        convertible_compartment_as_freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = 35,
        convertible_compartment_as_fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 2,
        convertible_compartment_as_freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 25
      },
      dwell = {
        dwell_time_in_minutes = 7,
        dwell_fresh_food_damper_position = 'closed',
        dwell_sealed_system_valve_position = 'position_B'
      },
      post_dwell = {
        post_dwell_exit_time_in_minutes = 10,
        secondary_only_post_dwell_exit_time_in_minutes = 10,
        post_dwell_freezer_evap_exit_temperature_in_degfx100 = -1000,
        secondary_only_post_dwell_freezer_evap_exit_temperature_in_degfx100 = -1200,
        post_dwell_sealed_system_valve_position = 'position_B',
        post_dwell_compressor_speed = 'low_speed',
        post_dwell_condenser_fan_speed = 'low_speed',
        post_dwell_fresh_food_damper_position = 'closed'
      }
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(defrost, generate_config())
  end)

  it('should assert if freezer_door_increment_factor_in_seconds_per_second is not in range', function()
    should_fail_with('freezer_door_increment_factor_in_seconds_per_second=-1 must be in [0, 65535]', function()
      defrost(generate_config({
        idle = {
          freezer_door_increment_factor_in_seconds_per_second = -1
        }
      }))
    end)
  end)

  it('should assert if fresh_food_door_increment_factor_in_seconds_per_second is not in range', function()
    should_fail_with('fresh_food_door_increment_factor_in_seconds_per_second=-1 must be in [0, 65535]', function()
      defrost(generate_config({
        idle = {
          fresh_food_door_increment_factor_in_seconds_per_second = -1
        }
      }))
    end)
  end)

  it('should assert if minimum_time_between_defrosts_abnormal_run_time_in_minutes is not in range', function()
    should_fail_with('minimum_time_between_defrosts_abnormal_run_time_in_minutes=-1 must be in [0, 65535]', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if max_time_between_defrosts_in_minutes is not in range', function()
    should_fail_with('max_time_between_defrosts_in_minutes=-1 must be in [0, 65535]', function()
      defrost(generate_config({
        idle = {
          max_time_between_defrosts_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if aham_prechill_time_between_defrosts_in_minutes is not in range', function()
    should_fail_with('aham_prechill_time_between_defrosts_in_minutes=-1 must be in [0, 65535]', function()
      defrost(generate_config({
        idle = {
          aham_prechill_time_between_defrosts_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if max_time_between_defrosts_in_minutes is equal to minimum_time_between_defrosts_abnormal_run_time_in_minutes', function()
    should_fail_with('max time between defrosts must be greater than minimum time between defrosts abnormal run time', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 5,
          max_time_between_defrosts_in_minutes = 5
        }
      }))
    end)
  end)

  it('should assert if max_time_between_defrosts_in_minutes is less than minimum_time_between_defrosts_abnormal_run_time_in_minutes', function()
    should_fail_with('max time between defrosts must be greater than minimum time between defrosts abnormal run time', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 5,
          max_time_between_defrosts_in_minutes = 4
        }
      }))
    end)
  end)

  it('should assert if post_dwell.post_dwell_exit_time_in_minutes is greater than idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes', function()
    should_fail_with('post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4
        },
        post_dwell = {
          post_dwell_exit_time_in_minutes = 5
        }
      }))
    end)
  end)

  it('should assert if post_dwell.post_dwell_exit_time_in_minutes is equal to idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes', function()
    should_fail_with('post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4
        },
        post_dwell = {
          post_dwell_exit_time_in_minutes = 4
        }
      }))
    end)
  end)

  it('should assert if post_dwell.post_dwell_exit_time_in_minutes is greater than idle.max_time_between_defrosts_in_minutes', function()
    should_fail_with('post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4,
          max_time_between_defrosts_in_minutes = 5
        },
        post_dwell = {
          post_dwell_exit_time_in_minutes = 6
        }
      }))
    end)
  end)

  it('should assert if post_dwell.post_dwell_exit_time_in_minutes is equal to idle.max_time_between_defrosts_in_minutes', function()
    should_fail_with('post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4,
          max_time_between_defrosts_in_minutes = 5
        },
        post_dwell = {
          post_dwell_exit_time_in_minutes = 5
        }
      }))
    end)
  end)

  it('should assert if post_dwell.secondary_only_post_dwell_exit_time_in_minutes is greater than idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes', function()
    should_fail_with('fresh food only post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4,
        },
        post_dwell = {
          secondary_only_post_dwell_exit_time_in_minutes = 5
        }
      }))
    end)
  end)

  it('should assert if post_dwell.secondary_only_post_dwell_exit_time_in_minutes is equal to idle.minimum_time_between_defrosts_abnormal_run_time_in_minutes', function()
    should_fail_with('fresh food only post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4
        },
        post_dwell = {
          secondary_only_post_dwell_exit_time_in_minutes = 4
        }
      }))
    end)
  end)

  it('should assert if post_dwell.secondary_only_post_dwell_exit_time_in_minutes is greater than idle.max_time_between_defrosts_in_minutes', function()
    should_fail_with('fresh food only post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4,
          max_time_between_defrosts_in_minutes = 5
        },
        post_dwell = {
          secondary_only_post_dwell_exit_time_in_minutes = 6
        }
      }))
    end)
  end)

  it('should assert if post_dwell.secondary_only_post_dwell_exit_time_in_minutes is equal to idle.max_time_between_defrosts_in_minutes', function()
    should_fail_with('fresh food only post dwell time should be less than minimum or maximum time between defrosts', function()
      defrost(generate_config({
        idle = {
          minimum_time_between_defrosts_abnormal_run_time_in_minutes = 4,
          max_time_between_defrosts_in_minutes = 5
        },
        post_dwell = {
          secondary_only_post_dwell_exit_time_in_minutes = 5
        }
      }))
    end)
  end)

  it('should assert if number_of_secondary_only_defrosts_before_full_defrost is not in range', function()
    should_fail_with('number_of_secondary_only_defrosts_before_full_defrost=-1 must be in [0, 255]', function()
      defrost(generate_config({
        prechill_prep = {
          number_of_secondary_only_defrosts_before_full_defrost = -1
        }
      }))
    end)
  end)

  it('should assert if number_of_secondary_only_defrosts_before_full_defrost_when_abnormal_is_set is not in range', function()
    should_fail_with('number_of_secondary_only_defrosts_before_full_defrost_when_abnormal_is_set=-1 must be in [0, 255]', function()
      defrost(generate_config({
        prechill_prep = {
          number_of_secondary_only_defrosts_before_full_defrost_when_abnormal_is_set = -1
        }
      }))
    end)
  end)

  it('should assert if max_prechill_prep_time_in_minutes is not in range', function()
    should_fail_with('max_prechill_prep_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        prechill_prep = {
          max_prechill_prep_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if max_prechill_time_in_minutes is not in range', function()
    should_fail_with('max_prechill_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        prechill = {
          max_prechill_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if max_prechill_time_for_secondary_only_defrost_in_minutes is not in range', function()
    should_fail_with('max_prechill_time_for_secondary_only_defrost_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        prechill = {
          max_prechill_time_for_secondary_only_defrost_in_minutes = -1
        }
      }))
    end)
  end)

  it('should require sealed system valve position argument of prechill to be valid sealed system valve position', function()
    should_fail_with("prechill.prechill_sealed_system_valve_position='not a valid position' must be in the set { 'position_home', 'position_A', 'position_B', 'position_C', 'position_D' }", function()
      local config = generate_config()
      config.prechill['prechill_sealed_system_valve_position'] = 'not a valid position'
      defrost(config)
    end)
  end)

  it('should require sealed system valve position argument of heater on entry to be valid sealed system valve position', function()
    should_fail_with("heater_on_entry.heater_on_entry_sealed_system_valve_position='not a valid position' must be in the set { 'position_home', 'position_A', 'position_B', 'position_C', 'position_D' }", function()
      local config = generate_config()
      config.heater_on_entry['heater_on_entry_sealed_system_valve_position'] = 'not a valid position'
      defrost(config)
    end)
  end)

  it('should require sealed system valve position argument of dwell to be valid sealed system valve position', function()
    should_fail_with("dwell.dwell_sealed_system_valve_position='not a valid position' must be in the set { 'position_home', 'position_A', 'position_B', 'position_C', 'position_D' }", function()
      local config = generate_config()
      config.dwell['dwell_sealed_system_valve_position'] = 'not a valid position'
      defrost(config)
    end)
  end)

  it('should require sealed system valve position argument of post dwell to be valid sealed system valve position', function()
    should_fail_with("post_dwell.post_dwell_sealed_system_valve_position='not a valid position' must be in the set { 'position_home', 'position_A', 'position_B', 'position_C', 'position_D' }", function()
      local config = generate_config()
      config.post_dwell['post_dwell_sealed_system_valve_position'] = 'not a valid position'
      defrost(config)
    end)
  end)

  it('should require compressor speed argument of prechill to be valid compressor speed', function()
    should_fail_with("prechill.prechill_compressor_speed='not a valid compressor speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.prechill['prechill_compressor_speed'] = 'not a valid compressor speed'
      defrost(config)
    end)
  end)

  it('should require compressor speed argument of post dwell to be valid compressor speed', function()
    should_fail_with("post_dwell.post_dwell_compressor_speed='not a valid compressor speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.post_dwell['post_dwell_compressor_speed'] = 'not a valid compressor speed'
      defrost(config)
    end)
  end)

  it('should require freezer evap fan speed argument of prechill to be valid fan speed', function()
    should_fail_with("prechill.prechill_freezer_evap_fan_speed='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.prechill['prechill_freezer_evap_fan_speed'] = 'not a valid fan speed'
      defrost(config)
    end)
  end)

  it('should require fresh food evap fan speed argument of prechill to be valid fan speed', function()
    should_fail_with("prechill.prechill_fresh_food_evap_fan_speed='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.prechill['prechill_fresh_food_evap_fan_speed'] = 'not a valid fan speed'
      defrost(config)
    end)
  end)

  it('should require ice box fan speed argument of prechill to be valid fan speed', function()
    should_fail_with("prechill.prechill_ice_cabinet_fan_speed='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.prechill['prechill_ice_cabinet_fan_speed'] = 'not a valid fan speed'
      defrost(config)
    end)
  end)

  it('should require deli fan speed argument of prechill to be valid fan speed', function()
    should_fail_with("prechill.prechill_deli_fan_speed='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.prechill['prechill_deli_fan_speed'] = 'not a valid fan speed'
      defrost(config)
    end)
  end)

  it('should require convertible compartment fan speed argument of prechill to be valid fan speed', function()
    should_fail_with("prechill.prechill_convertible_compartment_fan_speed='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.prechill['prechill_convertible_compartment_fan_speed'] = 'not a valid fan speed'
      defrost(config)
    end)
  end)

  it('should require condenser fan speed argument of post dwell to be valid fan speed', function()
    should_fail_with("post_dwell.post_dwell_condenser_fan_speed='not a valid fan speed' must be in the set { 'off_speed', 'super_low_speed', 'low_speed', 'medium_speed', 'high_speed', 'super_high_speed' }", function()
      local config = generate_config()
      config.post_dwell['post_dwell_condenser_fan_speed'] = 'not a valid fan speed'
      defrost(config)
    end)
  end)

  it('should require convertible compartment damper position argument of prechill to be valid damper position', function()
    should_fail_with("prechill.prechill_convertible_compartment_damper_position='not a valid damper position' must be in the set { 'closed', 'open' }", function()
      local config = generate_config()
      config.prechill['prechill_convertible_compartment_damper_position'] = 'not a valid damper position'
      defrost(config)
    end)
  end)

  it('should require fresh food damper position argument of prechill to be valid damper position', function()
    should_fail_with("prechill.prechill_fresh_food_damper_position='not a valid damper position' must be in the set { 'closed', 'open' }", function()
      local config = generate_config()
      config.prechill['prechill_fresh_food_damper_position'] = 'not a valid damper position'
      defrost(config)
    end)
  end)

  it('should require fresh food damper position argument of heater on entry to be valid damper position', function()
    should_fail_with("heater_on_entry.heater_on_entry_fresh_food_damper_position='not a valid damper position' must be in the set { 'closed', 'open' }", function()
      local config = generate_config()
      config.heater_on_entry['heater_on_entry_fresh_food_damper_position'] = 'not a valid damper position'
      defrost(config)
    end)
  end)

  it('should require fresh food damper position argument of dwell to be valid damper position', function()
    should_fail_with("dwell.dwell_fresh_food_damper_position='not a valid damper position' must be in the set { 'closed', 'open' }", function()
      local config = generate_config()
      config.dwell['dwell_fresh_food_damper_position'] = 'not a valid damper position'
      defrost(config)
    end)
  end)

  it('should require fresh food damper position argument of post dwell to be valid damper position', function()
    should_fail_with("post_dwell.post_dwell_fresh_food_damper_position='not a valid damper position' must be in the set { 'closed', 'open' }", function()
      local config = generate_config()
      config.post_dwell['post_dwell_fresh_food_damper_position'] = 'not a valid damper position'
      defrost(config)
    end)
  end)

  it('should assert if prechill_freezer_min_temp_in_degfx100 is not in range', function()
    should_fail_with('prechill_freezer_min_temp_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_freezer_min_temp_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if prechill_fresh_food_min_temp_in_degfx100 is not in range', function()
    should_fail_with('prechill_fresh_food_min_temp_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_fresh_food_min_temp_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if prechill_fresh_food_max_temp_in_degfx100 is not in range', function()
    should_fail_with('prechill_fresh_food_max_temp_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_fresh_food_max_temp_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if prechill_convertible_compartment_as_freezer_min_temp_in_degfx100 is not in range', function()
    should_fail_with('prechill_convertible_compartment_as_freezer_min_temp_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_convertible_compartment_as_freezer_min_temp_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100 is not in range', function()
    should_fail_with('prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100 is not in range', function()
    should_fail_with('prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if prechill_freezer_evap_exit_temperature_in_degfx100 is not in range', function()
    should_fail_with('prechill_freezer_evap_exit_temperature_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_freezer_evap_exit_temperature_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if prechill_convertible_compartment_evap_exit_temperature_in_degfx100 is not in range', function()
    should_fail_with('prechill_convertible_compartment_evap_exit_temperature_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        prechill = {
          prechill_convertible_compartment_evap_exit_temperature_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if defrost_heater_on_delay_after_compressor_off_in_seconds is not in range', function()
    should_fail_with('defrost_heater_on_delay_after_compressor_off_in_seconds=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on_entry = {
          defrost_heater_on_delay_after_compressor_off_in_seconds = -1
        }
      }))
    end)
  end)

  it('should assert if freezer_defrost_heater_max_on_time_in_minutes is not in range', function()
    should_fail_with('freezer_defrost_heater_max_on_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          freezer_defrost_heater_max_on_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes is not in range', function()
    should_fail_with('freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if freezer_heater_on_time_to_set_abnormal_defrost_in_minutes is not in range', function()
    should_fail_with('freezer_heater_on_time_to_set_abnormal_defrost_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if freezer_defrost_termination_temperature_in_degfx100 is not in range', function()
    should_fail_with('freezer_defrost_termination_temperature_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        heater_on = {
          freezer_defrost_termination_temperature_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if secondary_only_defrost_termination_temperature_in_degfx100 is not in range', function()
    should_fail_with('secondary_only_defrost_termination_temperature_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        heater_on = {
          secondary_only_defrost_termination_temperature_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if convertible_compartment_defrost_termination_temperature_in_degfx100 is not in range', function()
    should_fail_with('convertible_compartment_defrost_termination_temperature_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        heater_on = {
          convertible_compartment_defrost_termination_temperature_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if fresh_food_defrost_heater_max_on_time_in_minutes is not in range', function()
    should_fail_with('fresh_food_defrost_heater_max_on_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          fresh_food_defrost_heater_max_on_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes is not in range', function()
    should_fail_with('fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes is not in range', function()
    should_fail_with('fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if convertible_compartment_defrost_heater_max_on_time_in_minutes is not in range', function()
    should_fail_with('convertible_compartment_defrost_heater_max_on_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          convertible_compartment_defrost_heater_max_on_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if convertible_compartment_as_fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes is not in range', function()
    should_fail_with('convertible_compartment_as_fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          convertible_compartment_as_fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if convertible_compartment_as_freezer_heater_on_time_to_set_abnormal_defrost_in_minutes is not in range', function()
    should_fail_with('convertible_compartment_as_freezer_heater_on_time_to_set_abnormal_defrost_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          convertible_compartment_as_freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if convertible_compartment_as_fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes is not in range', function()
    should_fail_with('convertible_compartment_as_fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          convertible_compartment_as_fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if convertible_compartment_as_freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes is not in range', function()
    should_fail_with('convertible_compartment_as_freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        heater_on = {
          convertible_compartment_as_freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if dwell_time_in_minutes is not in range', function()
    should_fail_with('dwell_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        dwell = {
          dwell_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if post_dwell_exit_time_in_minutes is not in range', function()
    should_fail_with('post_dwell_exit_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        post_dwell = {
          post_dwell_exit_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if secondary_only_post_dwell_exit_time_in_minutes is not in range', function()
    should_fail_with('secondary_only_post_dwell_exit_time_in_minutes=-1 must be in [0, 255]', function()
      defrost(generate_config({
        post_dwell = {
          secondary_only_post_dwell_exit_time_in_minutes = -1
        }
      }))
    end)
  end)

  it('should assert if post_dwell_freezer_evap_exit_temperature_in_degfx100 is not in range', function()
    should_fail_with('post_dwell_freezer_evap_exit_temperature_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        post_dwell = {
          post_dwell_freezer_evap_exit_temperature_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should assert if secondary_only_post_dwell_freezer_evap_exit_temperature_in_degfx100 is not in range', function()
    should_fail_with('secondary_only_post_dwell_freezer_evap_exit_temperature_in_degfx100=-100000 must be in [-32768, 32767]', function()
      defrost(generate_config({
        post_dwell = {
          secondary_only_post_dwell_freezer_evap_exit_temperature_in_degfx100 = -100000
        }
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type defrost', function()
    local expected = remove_whitespace([[
      structure(
        structure(
          u16(348),
          u16(87),
          u16(360),
          u16(1920),
          u16(360)
        ),
        structure(
          u8(2),
          u8(1),
          u8(0)
        ),
        structure(
          u8(10),
          u8(20),
          u8(]] .. sealed_system_valve_position_type.position_B .. [[),
          u8(]] .. compressor_speed_type.low_speed .. [[),
          u8(]] .. fan_speed_type.super_low_speed .. [[),
          u8(]] .. fan_speed_type.low_speed .. [[),
          u8(]] .. fan_speed_type.medium_speed .. [[),
          u8(]] .. fan_speed_type.high_speed .. [[),
          u8(]] .. damper_position_type.open .. [[),
          u8(]] .. fan_speed_type.super_low_speed .. [[),
          u8(]] .. damper_position_type.closed .. [[),
          i16(-600),
          i16(3200),
          i16(4600),
          i16(-600),
          i16(3200),
          i16(4600),
          i16(-3000),
          i16(-3000)
        ),
        structure(
          u8(2),
          u8(]] .. damper_position_type.closed .. [[),
          u8(]] .. sealed_system_valve_position_type.position_A .. [[)
        ),
        structure(
          u8(60),
          u8(30),
          u8(32),
          i16(5900),
          i16(4460),
          i16(4460),
          u8(60),
          u8(20),
          u8(21),
          u8(60),
          u8(21),
          u8(35),
          u8(2),
          u8(25)
        ),
        structure(
          u8(7),
          u8(]] .. damper_position_type.closed .. [[),
          u8(]] .. sealed_system_valve_position_type.position_B .. [[)
        ),
        structure(
          u8(10),
          u8(10),
          i16(-1000),
          i16(-1200),
          u8(]] .. sealed_system_valve_position_type.position_B .. [[),
          u8(]] .. compressor_speed_type.low_speed .. [[),
          u8(]] .. fan_speed_type.low_speed .. [[),
          u8(]] .. damper_position_type.closed .. [[)
        )
      )
    ]])

    local actual = defrost({
      idle = {
        freezer_door_increment_factor_in_seconds_per_second = 348,
        fresh_food_door_increment_factor_in_seconds_per_second = 87,
        minimum_time_between_defrosts_abnormal_run_time_in_minutes = 6 * 60,
        max_time_between_defrosts_in_minutes = 32 * 60,
        aham_prechill_time_between_defrosts_in_minutes = 6 * 60
      },
      prechill_prep = {
        number_of_secondary_only_defrosts_before_full_defrost = 2,
        number_of_secondary_only_defrosts_before_full_defrost_when_abnormal_is_set = 1,
        max_prechill_prep_time_in_minutes = 0
      },
      prechill = {
        max_prechill_time_in_minutes = 10,
        max_prechill_time_for_secondary_only_defrost_in_minutes = 20,
        prechill_sealed_system_valve_position = 'position_B',
        prechill_compressor_speed = 'low_speed',
        prechill_freezer_evap_fan_speed = 'super_low_speed',
        prechill_fresh_food_evap_fan_speed = 'low_speed',
        prechill_ice_cabinet_fan_speed = 'medium_speed',
        prechill_deli_fan_speed = 'high_speed',
        prechill_convertible_compartment_damper_position = 'open',
        prechill_convertible_compartment_fan_speed = 'super_low_speed',
        prechill_fresh_food_damper_position = 'closed',
        prechill_freezer_min_temp_in_degfx100 = -600,
        prechill_fresh_food_min_temp_in_degfx100 = 3200,
        prechill_fresh_food_max_temp_in_degfx100 = 4600,
        prechill_convertible_compartment_as_freezer_min_temp_in_degfx100 = -600,
        prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100 = 3200,
        prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100 = 4600,
        prechill_freezer_evap_exit_temperature_in_degfx100 = -3000,
        prechill_convertible_compartment_evap_exit_temperature_in_degfx100 = -3000
      },
      heater_on_entry = {
        defrost_heater_on_delay_after_compressor_off_in_seconds = 2,
        heater_on_entry_fresh_food_damper_position = 'closed',
        heater_on_entry_sealed_system_valve_position = 'position_A'
      },
      heater_on = {
        freezer_defrost_heater_max_on_time_in_minutes = 60,
        freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 30,
        freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = 32,
        freezer_defrost_termination_temperature_in_degfx100 = 5900,
        secondary_only_defrost_termination_temperature_in_degfx100 = 4460,
        convertible_compartment_defrost_termination_temperature_in_degfx100 = 4460,
        fresh_food_defrost_heater_max_on_time_in_minutes = 60,
        fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 20,
        fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = 21,
        convertible_compartment_defrost_heater_max_on_time_in_minutes = 60,
        convertible_compartment_as_fresh_food_heater_on_time_to_set_abnormal_defrost_in_minutes = 21,
        convertible_compartment_as_freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = 35,
        convertible_compartment_as_fresh_food_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 2,
        convertible_compartment_as_freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 25
      },
      dwell = {
        dwell_time_in_minutes = 7,
        dwell_fresh_food_damper_position = 'closed',
        dwell_sealed_system_valve_position = 'position_B'
      },
      post_dwell = {
        post_dwell_exit_time_in_minutes = 10,
        secondary_only_post_dwell_exit_time_in_minutes = 10,
        post_dwell_freezer_evap_exit_temperature_in_degfx100 = -1000,
        secondary_only_post_dwell_freezer_evap_exit_temperature_in_degfx100 = -1200,
        post_dwell_sealed_system_valve_position = 'position_B',
        post_dwell_compressor_speed = 'low_speed',
        post_dwell_condenser_fan_speed = 'low_speed',
        post_dwell_fresh_food_damper_position = 'closed'
      }
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('defrost'))
  end)

  it('should memoize', function()
    should_memoize_calls(defrost, generate_config())
  end)
end)

return defrost({
  idle = {
    freezer_door_increment_factor_in_seconds_per_second = 348,
    fresh_food_door_increment_factor_in_seconds_per_second = 87,
    minimum_time_between_defrosts_abnormal_run_time_in_minutes = 6 * 60,
    max_time_between_defrosts_in_minutes = 32 * 60,
    aham_prechill_time_between_defrosts_in_minutes = 6 * 60
  },
  prechill_prep = {
    number_of_fresh_food_defrosts_before_freezer_defrost = 2,
    number_of_fresh_food_defrosts_before_abnormal_freezer_defrost = 1,
    max_prechill_prep_time_in_minutes = 5,
  },
  prechill = {
    max_prechill_time_in_minutes = 10,
    max_prechill_time_for_fresh_food_only_defrost_in_minutes = 20,
    prechill_refrigerant_valve_position = 'position_B',
    prechill_compressor_speed = 'low_speed',
    prechill_freezer_evap_fan_speed = 'super_low_speed',
    prechill_fresh_food_evap_fan_speed = 'low_speed',
    prechill_ice_box_fan_speed = 'medium_speed',
    prechill_deli_fan_speed = 'high_speed',
    prechill_convertible_compartment_damper_position = 'open',
    prechill_convertible_compartment_fan_speed = 'high_speed',
    prechill_fresh_food_damper_position = 'closed',
    prechill_freezer_min_temp_in_degfx100 = -600,
    prechill_fresh_food_min_temp_in_degfx100 = 3200,
    prechill_fresh_food_max_temp_in_degfx100 = 4600,
    prechill_convertible_compartment_as_freezer_min_temp_in_degfx100 = -600,
    prechill_convertible_compartment_as_fresh_food_min_temp_in_degfx100 = 3200,
    prechill_convertible_compartment_as_fresh_food_max_temp_in_degfx100 = 4600,
    prechill_freezer_evap_exit_temperature_in_degfx100 = -3000,
    prechill_convertible_compartment_evap_exit_temperature_in_degfx100 = -3000,
  },
  heater_on_entry = {
    defrost_heater_on_delay_after_compressor_off_in_seconds = 2,
    heater_on_entry_fresh_food_damper_position = 'closed'
  },
  heater_on = {
    defrost_heater_on_refrigerant_valve_position = 'position_A',
    freezer_defrost_heater_max_on_time_in_minutes = 60,
    freezer_invalid_thermistor_defrost_heater_max_on_time_in_minutes = 30,
    freezer_heater_on_time_to_set_abnormal_defrost_in_minutes = 32,
    freezer_defrost_termination_temperature_in_degfx100 = 5900,
    fresh_food_defrost_termination_temperature_in_degfx100 = 4460,
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
    dwell_refrigerant_valve_position = 'position_B',
  },
  post_dwell = {
    post_dwell_exit_time_in_minutes = 10,
    fresh_food_only_post_dwell_exit_time_in_minutes = 10,
    post_dwell_freezer_evap_exit_temperature_in_degfx100 = -1000,
    fresh_food_only_post_dwell_freezer_evap_exit_temperature_in_degfx100 = -1200,
    post_dwell_refrigerant_valve_position = 'position_B',
    post_dwell_compressor_speed = 'low_speed',
    post_dwell_condenser_fan_speed = 'low_speed',
    post_dwell_fresh_food_damper_position = 'closed'
  }
})

return sensors({
  freezer_cabinet_thermistor = import("../Sensor/sensorTypes/freezer_cabinet_thermistor.lua"),
  fresh_food_cabinet_thermistor = import("../Sensor/sensorTypes/fresh_food_cabinet_thermistor.lua"),
  freezer_evap_thermistor = import("../Sensor/sensorTypes/freezer_evap_thermistor.lua"),
  fresh_food_evap_thermistor = import("../Sensor/sensorTypes/fresh_food_evap_thermistor.lua"),
  convertible_compartment_cabinet_thermistor = import("../Sensor/sensorTypes/convertible_compartment_cabinet_thermistor.lua"),
  ambient_thermistor = import("../Sensor/sensorTypes/ambient_thermistor.lua"),
  ambient_humidity_sensor = import("../Sensor/sensorTypes/ambient_humidity_sensor.lua"),
  convertible_compartment_evap_thermistor = import("../Sensor/sensorTypes/convertible_compartment_evap_thermistor.lua"),
  ice_maker_0_mold_thermistor = import("../Sensor/sensorTypes/ice_maker_0_mold_thermistor.lua"),
  ice_maker_1_mold_thermistor = import("../Sensor/sensorTypes/ice_maker_1_mold_thermistor.lua"),
  ice_maker_2_mold_thermistor = import("../Sensor/sensorTypes/ice_maker_2_mold_thermistor.lua"),
  periodic_update_rate_in_msec = 1000
})

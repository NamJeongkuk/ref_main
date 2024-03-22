return sensors({
  freezer_cabinet_thermistor = import("../Sensor/sensorTypes/swqa/freezer_cabinet_thermistor_qa.lua"),
  fresh_food_cabinet_thermistor = import("../Sensor/sensorTypes/swqa/fresh_food_cabinet_thermistor_qa.lua"),
  ice_cabinet_thermistor = import("../Sensor/sensorTypes/swqa/ice_cabinet_thermistor_qa.lua"),
  convertible_compartment_as_freezer = import("../Sensor/sensorTypes/swqa/convertible_compartment_as_freezer_qa.lua"),
  convertible_compartment_as_fresh_food = import("../Sensor/sensorTypes/swqa/convertible_compartment_as_fresh_food_qa.lua"),
  freezer_evap_thermistor = import("../Sensor/sensorTypes/swqa/freezer_evap_thermistor_qa.lua"),
  fresh_food_evap_thermistor = import("../Sensor/sensorTypes/swqa/fresh_food_evap_thermistor_qa.lua"),
  ambient_thermistor = import("../Sensor/sensorTypes/swqa/ambient_thermistor_qa.lua"),
  ambient_humidity_sensor = import("../Sensor/sensorTypes/swqa/ambient_humidity_sensor_qa.lua"),
  convertible_compartment_evap_thermistor = import("../Sensor/sensorTypes/swqa/convertible_compartment_evap_thermistor_qa.lua"),
  ice_maker_0_mold_thermistor = import("../Sensor/sensorTypes/swqa/twist_tray_ice_maker_thermistor_qa.lua"),
  ice_maker_1_mold_thermistor = import("../Sensor/sensorTypes/swqa/aluminum_ice_maker_mold_thermistor_qa.lua"),
  ice_maker_2_mold_thermistor = import("../Sensor/sensorTypes/swqa/twist_tray_ice_maker_thermistor_qa.lua"),
  periodic_update_rate_in_msec = 1000
})

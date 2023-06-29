return sensors({
  freezer_cabinet_thermistor = import("../Sensor/sensorTypes/swqa/freezer_cabinet_thermistor_qa.lua"),
  fresh_food_cabinet_thermistor = import("../Sensor/sensorTypes/swqa/fresh_food_cabinet_thermistor_qa.lua"),
  freezer_evap_thermistor = import("../Sensor/sensorTypes/swqa/freezer_evap_thermistor_qa.lua"),
  fresh_food_evap_thermistor = import("../Sensor/sensorTypes/fresh_food_evap_thermistor.lua"),
  convertible_compartment_cabinet_thermistor = import("../Sensor/sensorTypes/convertible_compartment_cabinet_thermistor.lua"),
  ambient_thermistor = import("../Sensor/sensorTypes/swqa/ambient_thermistor_qa.lua"),
  ambient_humidity_thermistor = import("../Sensor/sensorTypes/swqa/ambient_humidity_thermistor_qa.lua"),
  convertible_compartment_evap_thermistor = import("../Sensor/sensorTypes/convertible_compartment_evap_thermistor.lua"),
  aluminum_ice_maker_mold_thermistor = import("../Sensor/sensorTypes/aluminum_ice_maker_mold_thermistor.lua"),
  twist_tray_ice_maker_thermistor = import("../Sensor/sensorTypes/twist_tray_ice_maker_thermistor.lua"),
  periodic_update_rate_in_msec = 1000
})

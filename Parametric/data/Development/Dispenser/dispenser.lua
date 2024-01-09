return dispenser({
  maximum_dispensed_ouncesx100 = 12800,
  maximum_dispense_time_in_seconds = 420,
  door_inhibit_water_dispense_table = import("../Dispenser/all_doors_dispense_inhibited.lua"),
  door_inhibit_ice_dispense_table = import("../Dispenser/all_doors_dispense_not_inhibited.lua"),
  auger_motor = import("../IceMaker/auger_motor.lua")
})

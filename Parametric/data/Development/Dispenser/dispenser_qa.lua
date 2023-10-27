return dispenser({
  maximum_dispensed_ouncesx100 = 2000,
  maximum_dispense_time_in_seconds = 30,
  door_inhibit_water_dispense_table = import("../Dispenser/all_doors_dispense_not_inhibited_qa.lua"),
  door_inhibit_ice_dispense_table = import("../Dispenser/all_doors_dispense_inhibited_qa.lua")
})

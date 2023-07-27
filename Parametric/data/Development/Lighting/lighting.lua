return lighting({
   maximum_light_duty_cycle_percentage = 100,
   fresh_food_backwall = import('../Lighting/fresh_food_back_wall_door_lighting_data.lua'),
   fresh_food_top_and_side = import('../Lighting/fresh_food_top_and_side_door_lighting_data.lua'),
   freezer_backwall = import('../Lighting/freezer_back_wall_door_lighting_data.lua'),
   freezer_top_and_side = import('../Lighting/freezer_top_and_side_door_lighting_data.lua')
})

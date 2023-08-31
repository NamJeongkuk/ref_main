local enum = require 'lua-common'.utilities.enum

-- Must match UserAllowableInteriorLighting_t enum
return enum({
  'fresh_food_back_light',
  'fresh_food_top_light',
  'freezer_back_light',
  'freezer_top_light'
}, 'interior_light_enum')

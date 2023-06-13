local enum = require 'lua-common'.utilities.enum

return enum({
  'off_speed',
  'super_low_speed',
  'low_speed',
  'medium_speed',
  'high_speed',
  'super_high_speed'
}, 'fan_speed_type')

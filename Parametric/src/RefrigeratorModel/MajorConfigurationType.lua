local enum = require 'lua-common'.utilities.enum

return enum({
  'single_evaporator_variable_speed_comp_sevs',
  'dual_evaporator_single_speed_comp_dess',
  'dual_evaporator_variable_speed_comp_devs',
  'single_evaporator_single_speed_comp_sess',
  'triple_evaporator_variable_speed_comp_tevs',
  'triple_evaporator_single_speed_comp_tess'
}, 'major_configuration_type')

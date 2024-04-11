local RefrigeratorModel = require 'RefrigeratorModel/RefrigeratorModel'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local refrigerator_model_type = require 'RefrigeratorModel.RefrigeratorModelType'
local major_configuration_type = require 'RefrigeratorModel.MajorConfigurationType'

describe('RefrigeratorModel', function()
  local refrigerator_model = RefrigeratorModel(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      refrigerator_model_type = 'side_by_side',
      major_configuration_type = 'single_evaporator_variable_speed_comp_sevs'
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(refrigerator_model, generate_config())
  end)

  it('should require refrigerator_model_type to be a valid refrigerator model type', function()
    should_fail_with("refrigerator_model_type='among_us_refrigerator' must be in the set { 'bottom_freezer_french_door', 'bottom_freezer_one_door', 'side_by_side', 'single_cabinet_fresh_food', 'single_cabinet_freezer', 'top_freezer', 'convertible_compartment_double_drawer_bottom_freezer', 'convertible_compartment_quad_door', 'f_and_p_wine_column', 'f_and_p_columns_b', 'dual_zone_wine_chiller', 'beverage_center', 'f_and_p_columns_fresh_food', 'f_and_p_columns_freezer', 'f_and_p_60cm_single_door', 'f_and_p_60cm_multi-door', 'f_and_p_rf605_platform', 'f_and_p_active_smart_freestanding', 'f_and_p_active_smart_built_in', 'haier_counter_depth_platform', 'convertible_compartment_single_fresh_food_door_with_double_drawer_bottom_freezer' }", function()
      refrigerator_model(generate_config({
        refrigerator_model_type = 'among_us_refrigerator'
      }))
    end)
  end)

  it('should require major_configuration_type to be a valid major configuration type', function()
    should_fail_with("major_configuration_type='among_us_refrigerator' must be in the set { 'single_evaporator_variable_speed_comp_sevs', 'dual_evaporator_single_speed_comp_dess', 'dual_evaporator_variable_speed_comp_devs', 'single_evaporator_single_speed_comp_sess', 'triple_evaporator_variable_speed_comp_tevs', 'triple_evaporator_single_speed_comp_tess' }", function()
      refrigerator_model(generate_config({
        major_configuration_type = 'among_us_refrigerator'
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type refrigerator_model', function()
    local expected = remove_whitespace([[
      structure(
        u8(]] .. refrigerator_model_type.side_by_side .. [[),
        u8(0),
        u8(]] .. major_configuration_type.single_evaporator_variable_speed_comp_sevs .. [[)
      )
    ]])

    local actual = refrigerator_model(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('refrigerator_model'))
  end)

  it('should memoize', function()
    should_memoize_calls(refrigerator_model, generate_config())
  end)

end)

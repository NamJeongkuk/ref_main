local Personality = require "Personality.Personality"
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_require_args = require 'lua-common'.utilities.should_require_args
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local TypedString = require 'lua-common'.util.TypedString

describe("Personality", function()
  local personality = Personality(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      refrigerator_model = TypedString('refrigerator_model', 'refrigerator_model'),
      platform = TypedString('platform', 'platform'),
      defrost = TypedString('defrost', 'defrost'),
      enhanced_sabbath = TypedString('enhanced_sabbath', 'enhanced_sabbath'),
      fans = TypedString('fan_list', 'fan_list'),
      fresh_food_and_freezer_grid = TypedString('fresh_food_and_freezer_grid', 'fresh_food_and_freezer_grid'),
      fresh_food_and_freezer_grid_deltas = TypedString('grid_deltas', 'grid_deltas'),
      fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = -20,
      fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 500,
      feature_pan_grid = TypedString('feature_pan_grid', 'feature_pan_grid'),
      feature_pan_grid_deltas = TypedString('grid_deltas', 'grid_deltas'),
      ice_cabinet_grid = TypedString('ice_cabinet_grid', 'ice_cabinet_grid'),
      ice_cabinet_grid_deltas = TypedString('grid_deltas', 'grid_deltas'),
      sabbath = TypedString('sabbath', 'sabbath'),
      convertible_compartment = TypedString('convertible_compartment', 'convertible_compartment'),
      feature_pan = TypedString('feature_pan', 'feature_pan'),
      ice_cabinet = TypedString('ice_cabinet', 'ice_cabinet'),
      sensors = TypedString('sensors', 'sensors'),
      system_monitor = TypedString('system_monitor', 'system_monitor'),
      compressor = TypedString('compressor', 'compressor'),
      setpoint = TypedString('setpoint', 'setpoint'),
      fresh_food_thermal_offset = TypedString('fresh_food_thermal_offset', 'fresh_food_thermal_offset'),
      freezer_thermal_offset = TypedString('freezer_thermal_offset', 'freezer_thermal_offset'),
      feature_pan_thermal_offset = TypedString('feature_pan_thermal_offset', 'feature_pan_thermal_offset'),
      ice_cabinet_thermal_offset = TypedString('ice_cabinet_thermal_offset', 'ice_cabinet_thermal_offset'),
      shift_offset_calculator = TypedString('shift_offset_calculator', 'shift_offset_calculator'),
      bsp_configuration = TypedString('bsp_configuration', 'bsp_configuration'),
      damper = TypedString('damper', 'damper'),
      damper_heater = TypedString('damper_heater', 'damper_heater'),
      pulldown = TypedString('pulldown', 'pulldown'),
      ice_maker = TypedString('ice_maker', 'ice_maker'),
      flow_meter = TypedString('flow_meter', 'flow_meter'),
      dispenser = TypedString('dispenser', 'dispenser'),
      recess_heater = TypedString('recess_heater', 'recess_heater'),
      turbo_mode_setpoint = TypedString('turbo_mode_setpoint', 'turbo_mode_setpoint'),
      lighting = TypedString('lighting', 'lighting'),
      load_off_door_open_compartment_list = TypedString('load_off_door_open_compartment_list', 'load_off_door_open_compartment_list'),
      filter_data = TypedString('filter_data', 'filter_data'),
      water_valve_flow_rate = TypedString('water_valve_flow_rate', 'water_valve_flow_rate'),
      cabinet_temperature_exceeded = TypedString('cabinet_temperature_exceeded', 'cabinet_temperature_exceeded'),
      sealed_system_valve = TypedString('sealed_system_valve', 'sealed_system_valve'),
      fresh_food_non_heated_cycle_defrost = TypedString('fresh_food_non_heated_cycle_defrost', 'fresh_food_non_heated_cycle_defrost')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(personality, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('refrigerator_model must be a typed string with type refrigerator_model, but is a number', function()
      personality(generate_config({
        refrigerator_model = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('platform must be a typed string with type platform, but is a number', function()
      personality(generate_config({
        platform = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('defrost must be a typed string with type defrost, but is a number', function()
      personality(generate_config({
        defrost = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('enhanced_sabbath must be a typed string with type enhanced_sabbath, but is a number', function()
      personality(generate_config({
        enhanced_sabbath = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('fans must be a typed string with type fan_list, but is a number', function()
      personality(generate_config({
        fans = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_and_freezer_grid must be a typed string with type fresh_food_and_freezer_grid, but is a number', function()
      personality(generate_config({
        fresh_food_and_freezer_grid = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_and_freezer_grid_deltas must be a typed string with type grid_deltas, but is a number', function()
      personality(generate_config({
        fresh_food_and_freezer_grid_deltas = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf=999999 must be in [0, 65535]', function()
      personality(generate_config({
        fresh_food_cross_ambient_hysteresis_coefficient_in_degfx1000_over_degf = 999999
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100=999999 must be in [-32768, 32767]', function()
      personality(generate_config({
        fresh_food_minimum_cross_ambient_adjusted_hysteresis_in_degfx100 = 999999
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('feature_pan_grid must be a typed string with type feature_pan_grid, but is a number', function()
      personality(generate_config({
        feature_pan_grid = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('feature_pan_grid_deltas must be a typed string with type grid_deltas, but is a number', function()
      personality(generate_config({
        feature_pan_grid_deltas = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('ice_cabinet_grid must be a typed string with type ice_cabinet_grid, but is a number', function()
      personality(generate_config({
        ice_cabinet_grid = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('ice_cabinet_grid_deltas must be a typed string with type grid_deltas, but is a number', function()
      personality(generate_config({
        ice_cabinet_grid_deltas = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('sabbath must be a typed string with type sabbath, but is a number', function()
      personality(generate_config({
        sabbath = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('convertible_compartment must be a typed string with type convertible_compartment, but is a number', function()
      personality(generate_config({
        convertible_compartment = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('feature_pan must be a typed string with type feature_pan, but is a number', function()
      personality(generate_config({
        feature_pan = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('ice_cabinet must be a typed string with type ice_cabinet, but is a number', function()
      personality(generate_config({
        ice_cabinet = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('sensors must be a typed string with type sensors, but is a number', function()
      personality(generate_config({
        sensors = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('system_monitor must be a typed string with type system_monitor, but is a number', function()
      personality(generate_config({
        system_monitor = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('compressor must be a typed string with type compressor, but is a number', function()
      personality(generate_config({
        compressor = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('setpoint must be a typed string with type setpoint, but is a number', function()
      personality(generate_config({
        setpoint = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_thermal_offset must be a typed string with type fresh_food_thermal_offset, but is a number', function()
      personality(generate_config({
        fresh_food_thermal_offset = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('freezer_thermal_offset must be a typed string with type freezer_thermal_offset, but is a number', function()
      personality(generate_config({
        freezer_thermal_offset = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('feature_pan_thermal_offset must be a typed string with type feature_pan_thermal_offset, but is a number', function()
      personality(generate_config({
        feature_pan_thermal_offset = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('ice_cabinet_thermal_offset must be a typed string with type ice_cabinet_thermal_offset, but is a number', function()
      personality(generate_config({
        ice_cabinet_thermal_offset = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('shift_offset_calculator must be a typed string with type shift_offset_calculator, but is a number', function()
      personality(generate_config({
        shift_offset_calculator = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('bsp_configuration must be a typed string with type bsp_configuration, but is a number', function()
      personality(generate_config({
        bsp_configuration = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('damper must be a typed string with type damper, but is a number', function()
      personality(generate_config({
        damper = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('damper_heater must be a typed string with type damper_heater, but is a number', function()
      personality(generate_config({
        damper_heater = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('pulldown must be a typed string with type pulldown, but is a number', function()
      personality(generate_config({
        pulldown = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('ice_maker must be a typed string with type ice_maker, but is a number', function()
      personality(generate_config({
        ice_maker = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('flow_meter must be a typed string with type flow_meter, but is a number', function()
      personality(generate_config({
        flow_meter = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('dispenser must be a typed string with type dispenser, but is a number', function()
      personality(generate_config({
        dispenser = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('recess_heater must be a typed string with type recess_heater, but is a number', function()
      personality(generate_config({
        recess_heater = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('turbo_mode_setpoint must be a typed string with type turbo_mode_setpoint, but is a number', function()
      personality(generate_config({
        turbo_mode_setpoint = -1
      }))
    end)
  end)

  it('should constain all arguments', function()
    should_fail_with('lighting must be a typed string with type lighting, but is a number', function()
      personality(generate_config({
        lighting = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('load_off_door_open_compartment_list must be a typed string with type load_off_door_open_compartment_list, but is a number', function()
      personality(generate_config({
        load_off_door_open_compartment_list = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('filter_data must be a typed string with type filter_data, but is a number', function()
      personality(generate_config({
        filter_data = -1
      }))
    end)
  end)

  it('should constain all arguments', function()
    should_fail_with('water_valve_flow_rate must be a typed string with type water_valve_flow_rate, but is a number', function()
      personality(generate_config({
        water_valve_flow_rate = -1
      }))
    end)
  end)

  it('should constain all arguments', function()
    should_fail_with('cabinet_temperature_exceeded must be a typed string with type cabinet_temperature_exceeded, but is a number', function()
      personality(generate_config({
        cabinet_temperature_exceeded = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('sealed_system_valve must be a typed string with type sealed_system_valve, but is a number', function()
      personality(generate_config({
        sealed_system_valve = -1
      }))
    end)
  end)

  it('should constrain all arguments', function()
    should_fail_with('fresh_food_non_heated_cycle_defrost must be a typed string with type fresh_food_non_heated_cycle_defrost, but is a number', function()
      personality(generate_config({
        fresh_food_non_heated_cycle_defrost = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type personality', function()
    local expected = remove_whitespace([[
      structure(
        pointer(refrigerator_model),
        pointer(platform),
        pointer(defrost),
        pointer(enhanced_sabbath),
        pointer(fan_list),
        pointer(fresh_food_and_freezer_grid),
        pointer(grid_deltas),
        i16(-20),
        u16(500),
        pointer(feature_pan_grid),
        pointer(grid_deltas),
        pointer(ice_cabinet_grid),
        pointer(grid_deltas),
        pointer(sabbath),
        pointer(convertible_compartment),
        pointer(feature_pan),
        pointer(ice_cabinet),
        pointer(sensors),
        pointer(system_monitor),
        pointer(compressor),
        pointer(setpoint),
        pointer(fresh_food_thermal_offset),
        pointer(freezer_thermal_offset),
        pointer(feature_pan_thermal_offset),
        pointer(ice_cabinet_thermal_offset),
        pointer(shift_offset_calculator),
        pointer(bsp_configuration),
        pointer(damper),
        pointer(damper_heater),
        pointer(pulldown),
        pointer(ice_maker),
        pointer(flow_meter),
        pointer(dispenser),
        pointer(recess_heater),
        pointer(turbo_mode_setpoint),
        pointer(lighting),
        pointer(load_off_door_open_compartment_list),
        pointer(filter_data),
        pointer(water_valve_flow_rate),
        pointer(cabinet_temperature_exceeded),
        pointer(sealed_system_valve),
        pointer(fresh_food_non_heated_cycle_defrost)
      )
    ]])

    local actual = personality(generate_config())

    assert(actual.is_of_type('parametric_personality'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)
end)

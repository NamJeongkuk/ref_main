local Sensors = require 'Sensor/Sensors'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('sensors', function()
  local sensors = Sensors(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
      fresh_food_cabinet_thermistor = TypedString('sensor_type', 'fresh_food_cabinet_thermistor'),
      freezer_evap_thermistor = TypedString('sensor_type', 'freezer_evap_thermistor'),
      fresh_food_evap_thermistor = TypedString('sensor_type', 'fresh_food_evap_thermistor'),
      convertible_compartment_cabinet_thermistor = TypedString('convertible_compartment_sensor_type', 'convertible_compartment_cabinet_thermistor'),
      ambient_thermistor = TypedString('sensor_type', 'ambient_thermistor'),
      ambient_humidity_sensor = TypedString('humidity_sensor_type', 'ambient_humidity_sensor'),
      convertible_compartment_evap_thermistor = TypedString('sensor_type', 'convertible_compartment_evap_thermistor'),
      aluminum_ice_maker_mold_thermistor = TypedString('sensor_type', 'aluminum_ice_maker_mold_thermistor'),
      twist_tray_ice_maker_thermistor = TypedString('sensor_type', 'twist_tray_ice_maker_thermistor'),
      periodic_update_rate_in_msec = 1000
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(sensors, generate_config())
  end)

  it('should constrain all arguments', function()
    should_fail_with('freezer_cabinet_thermistor must be a typed string with type sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = -1
      }))
    end)

    should_fail_with('fresh_food_cabinet_thermistor must be a typed string with type sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = -1
      }))
    end)

    should_fail_with('freezer_evap_thermistor must be a typed string with type sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = TypedString('sensor_type', 'fresh_food_cabinet_thermistor'),
        freezer_evap_thermistor = -1
      }))
    end)

    should_fail_with('fresh_food_evap_thermistor must be a typed string with type sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = TypedString('sensor_type', 'fresh_food_cabinet_thermistor'),
        freezer_evap_thermistor = TypedString('sensor_type', 'freezer_evap_thermistor'),
        fresh_food_evap_thermistor = -1
      }))
    end)

    should_fail_with('convertible_compartment_cabinet_thermistor must be a typed string with type convertible_compartment_sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = TypedString('sensor_type', 'fresh_food_cabinet_thermistor'),
        freezer_evap_thermistor = TypedString('sensor_type', 'freezer_evap_thermistor'),
        fresh_food_evap_thermistor = TypedString('sensor_type', 'fresh_food_evap_thermistor'),
        convertible_compartment_cabinet_thermistor = -1
      }))
    end)

    should_fail_with('ambient_thermistor must be a typed string with type sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = TypedString('sensor_type', 'fresh_food_cabinet_thermistor'),
        freezer_evap_thermistor = TypedString('sensor_type', 'freezer_evap_thermistor'),
        fresh_food_evap_thermistor = TypedString('sensor_type', 'fresh_food_evap_thermistor'),
        convertible_compartment_cabinet_thermistor = TypedString('convertible_compartment_sensor_type', 'convertible_compartment_cabinet_thermistor'),
        ambient_thermistor = -1
      }))
    end)

    should_fail_with('ambient_humidity_sensor must be a typed string with type humidity_sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = TypedString('sensor_type', 'fresh_food_cabinet_thermistor'),
        freezer_evap_thermistor = TypedString('sensor_type', 'freezer_evap_thermistor'),
        fresh_food_evap_thermistor = TypedString('sensor_type', 'fresh_food_evap_thermistor'),
        convertible_compartment_cabinet_thermistor = TypedString('convertible_compartment_sensor_type', 'convertible_compartment_cabinet_thermistor'),
        ambient_humidity_sensor = -1
      }))
    end)

    should_fail_with('convertible_compartment_evap_thermistor must be a typed string with type sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = TypedString('sensor_type','fresh_food_cabinet_thermistor'),
        freezer_evap_thermistor = TypedString('sensor_type', 'freezer_evap_thermistor'),
        fresh_food_evap_thermistor = TypedString('sensor_type', 'fresh_food_evap_thermistor'),
        convertible_compartment_cabinet_thermistor = TypedString('convertible_compartment_sensor_type', 'convertible_compartment_cabinet_thermistor'),
        ambient_thermistor = TypedString('sensor_type', 'ambient_thermistor'),
        convertible_compartment_evap_thermistor = -1
      }))
    end)

    should_fail_with('aluminum_ice_maker_mold_thermistor must be a typed string with type sensor_type, but is a number', function()
      sensors(generate_config({
        freezer_cabinet_thermistor = TypedString('sensor_type', 'freezer_cabinet_thermistor'),
        fresh_food_cabinet_thermistor = TypedString('sensor_type','fresh_food_cabinet_thermistor'),
        freezer_evap_thermistor = TypedString('sensor_type', 'freezer_evap_thermistor'),
        fresh_food_evap_thermistor = TypedString('sensor_type', 'fresh_food_evap_thermistor'),
        convertible_compartment_cabinet_thermistor = TypedString('convertible_compartment_sensor_type', 'convertible_compartment_cabinet_thermistor'),
        ambient_thermistor = TypedString('sensor_type', 'ambient_thermistor'),
        convertible_compartment_evap_thermistor = TypedString('sensor_type', 'convertible_compartment_evap_thermistor'),
        aluminum_ice_maker_mold_thermistor = -1
      }))
    end)
  end)

  it('should assert if periodic_update_rate_in_msec is not in range', function()
    should_fail_with('periodic_update_rate_in_msec=-1 must be in [0, 65535]', function()
      sensors(generate_config({
        periodic_update_rate_in_msec = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type sensors', function()
    local expected = remove_whitespace([[
        structure(
          pointer(freezer_cabinet_thermistor),
          pointer(fresh_food_cabinet_thermistor),
          pointer(freezer_evap_thermistor),
          pointer(fresh_food_evap_thermistor),
          pointer(convertible_compartment_cabinet_thermistor),
          pointer(ambient_thermistor),
          pointer(ambient_humidity_sensor),
          pointer(convertible_compartment_evap_thermistor),
          pointer(aluminum_ice_maker_mold_thermistor),
          pointer(twist_tray_ice_maker_thermistor),
          u16(1000)
      )
    ]])

    local actual = sensors(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('sensors'))
  end)

  it('should memoize', function()
    should_memoize_calls(sensors, generate_config())
  end)

end)

local FreezerSetpointToIceCabinetSetpointTable = require 'IceCabinet.FreezerSetpointToIceCabinetSetpointTable'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('FreezerSetpointToIceCabinetSetpointTable', function()
  local freezer_setpoint_to_ice_cabinet_setpoint_table = FreezerSetpointToIceCabinetSetpointTable(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      data = {
        { freezer = -600, ice_cabinet = 300 },
        { freezer = -500, ice_cabinet = 500 },
        { freezer = -400, ice_cabinet = 700 },
        { freezer = -300, ice_cabinet = 900 },
        { freezer = -200, ice_cabinet = 1100 },
        { freezer = -100, ice_cabinet = 1300 },
        { freezer = 0, ice_cabinet = 1500 },
        { freezer = 100, ice_cabinet = 1700 },
        { freezer = 200, ice_cabinet = 1900 },
        { freezer = 300, ice_cabinet = 2100 },
        { freezer = 400, ice_cabinet = 2300 },
        { freezer = 500, ice_cabinet = 2500 },
        { freezer = 600, ice_cabinet = 2700 },
        { freezer = 700, ice_cabinet = 2900 },
        { freezer = 800, ice_cabinet = 3100 }
      }
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(freezer_setpoint_to_ice_cabinet_setpoint_table, generate_config())
  end)

  it('should fail with incorrect arguments', function()
    should_fail_with('freezer setpoints must be in ascending order', function()
      freezer_setpoint_to_ice_cabinet_setpoint_table(generate_config({
        data = {
          { freezer = 100, ice_cabinet = 1300 },
          { freezer = 0, ice_cabinet = 900 },
          { freezer = 200, ice_cabinet = 1100 }
        }
      }))
    end)

    should_fail_with('data[1].freezer=-999999 must be in [-32768, 32767]', function()
      freezer_setpoint_to_ice_cabinet_setpoint_table(generate_config({
        data = {
          { freezer = -999999, ice_cabinet = 1300 }
        }
      }))
    end)

    should_fail_with('data[1].ice_cabinet=-999999 must be in [-32768, 32767]', function()
      freezer_setpoint_to_ice_cabinet_setpoint_table(generate_config({
        data = {
          { freezer = 100, ice_cabinet = -999999 }
        }
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type freezer_setpoint_to_ice_cabinet_setpoint_table', function()
    local expected = remove_whitespace([[
      structure(
        pointer(
          structure(
            structure(i16(-600), i16(300)),
            structure(i16(-500), i16(500)),
            structure(i16(-400), i16(700)),
            structure(i16(-300), i16(900)),
            structure(i16(-200), i16(1100)),
            structure(i16(-100), i16(1300)),
            structure(i16(0), i16(1500)),
            structure(i16(100), i16(1700)),
            structure(i16(200), i16(1900)),
            structure(i16(300), i16(2100)),
            structure(i16(400), i16(2300)),
            structure(i16(500), i16(2500)),
            structure(i16(600), i16(2700)),
            structure(i16(700), i16(2900)),
            structure(i16(800), i16(3100))
          )
        ),
        u8(15)
      )
    ]])

    local actual = freezer_setpoint_to_ice_cabinet_setpoint_table(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('freezer_setpoint_to_ice_cabinet_setpoint_table'))
  end)

  it('should memoize', function()
    should_memoize_calls(freezer_setpoint_to_ice_cabinet_setpoint_table, generate_config())
  end)
end)

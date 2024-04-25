local GammaTable = require 'IceCabinet/GammaTable'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('GammaTable', function()
  local gamma_table = GammaTable(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      ice_cabinet_med_speed_limit = 25,
      ice_cabinet_high_speed_limit = 50,
      ice_cabinet_super_high_speed_limit = 75,
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(gamma_table, generate_config())
  end)

  it('should assert if ice_cabinet_med_speed_limit is not in range', function()
    should_fail_with('ice_cabinet_med_speed_limit=-1 must be in [0, 255]', function()
      gamma_table(generate_config({
        ice_cabinet_med_speed_limit = -1
      }))
    end)
  end)

  it('should assert if ice_cabinet_high_speed_limit is not in range', function()
    should_fail_with('ice_cabinet_high_speed_limit=-1 must be in [0, 255]', function()
      gamma_table(generate_config({
        ice_cabinet_high_speed_limit = -1
      }))
    end)
  end)

  it('should assert if ice_cabinet_super_high_speed_limit is not in range', function()
    should_fail_with('ice_cabinet_super_high_speed_limit=-1 must be in [0, 255]', function()
      gamma_table(generate_config({
        ice_cabinet_super_high_speed_limit = -1
      }))
    end)
  end)

  it('should assert if the parameters are out of order', function()
    should_fail_with('gamma table limits must be in order and different', function()
      gamma_table(generate_config({
        ice_cabinet_med_speed_limit = 50
      }))
    end)

    should_fail_with('gamma table limits must be in order and different', function()
      gamma_table(generate_config({
        ice_cabinet_high_speed_limit = 75
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type gamma_table', function()
    local expected = remove_whitespace([[
      structure(
        u8(25),
        u8(50),
        u8(75)
      )
      ]])

    local actual = gamma_table({
      ice_cabinet_med_speed_limit = 25,
      ice_cabinet_high_speed_limit = 50,
      ice_cabinet_super_high_speed_limit = 75,
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('gamma_table'))
  end)

  it('should memoize', function()
    should_memoize_calls(gamma_table, generate_config())
  end)
end)

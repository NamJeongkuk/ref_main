local HighAmbientOffset = require 'Setpoints/HighAmbientOffset'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('HighAmbientOffset', function()
   local high_ambient_offset = HighAmbientOffset(core_mock)

   local function generate_config(overrides)
      return require 'lua-common'.table.merge({
         relative_humidity_trigger_for_high_ambient_humidity_offset_in_percent = 30,
         user_setpoint_trigger_for_high_ambient_humidity_offset_in_degf = 20,
         offset_for_high_ambient_humidity_in_degfx100 = 1000
      }, overrides or {})
   end

   it('should require all arguments', function()
      should_require_args(high_ambient_offset, generate_config())
    end)

    it('should assert if relative_humidity_trigger_for_high_ambient_humidity_offset_in_percent is not in range', function()
      should_fail_with('relative_humidity_trigger_for_high_ambient_humidity_offset_in_percent=-1 must be in [0, 100]', function()
        high_ambient_offset(generate_config({
         relative_humidity_trigger_for_high_ambient_humidity_offset_in_percent = -1
        }))
      end)
    end)

    it('should assert if user_setpoint_trigger_for_high_ambient_humidity_offset_in_degf is not in range', function()
      should_fail_with('user_setpoint_trigger_for_high_ambient_humidity_offset_in_degf=128 must be in [-128, 127]', function()
        high_ambient_offset(generate_config({
         user_setpoint_trigger_for_high_ambient_humidity_offset_in_degf = 128
        }))
      end)
    end)

    it('should assert if offset_for_high_ambient_humidity_in_degfx100 is not in range', function()
      should_fail_with('offset_for_high_ambient_humidity_in_degfx100=32768 must be in [-32768, 32767]', function()
        high_ambient_offset(generate_config({
         offset_for_high_ambient_humidity_in_degfx100= 32768
        }))
      end)
    end)

    it('should generate a typed string with the correct data and type for high ambient offset', function()
      local expected = remove_whitespace([[
        structure(
          u8(30),
          i8(20),
          i16(1000)
        )
      ]])

      local actual = high_ambient_offset({
        relative_humidity_trigger_for_high_ambient_humidity_offset_in_percent = 30,
        user_setpoint_trigger_for_high_ambient_humidity_offset_in_degf = 20,
        offset_for_high_ambient_humidity_in_degfx100 = 1000
      })

      assert.equals(expected, remove_whitespace(tostring(actual)))
      assert(actual.is_of_type('high_ambient_offset'))
    end)

    it('should memoize', function()
      should_memoize_calls(high_ambient_offset, generate_config())
    end)
end)

local Pid = require 'Fan/Pid'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('Pid', function()
  local pid = Pid(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      kp_value = 11,
      kp_bits = 12,
      ki_value = 13,
      ki_bits = 14,
      kd_value = 15,
      kd_bits = 16,
      lower_limit = 17,
      upper_limit = 18
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(pid, generate_config())
  end)

  it('should assert if kp_value is not in range', function()
    should_fail_with('kp_value=2147483648 must be in [-2147483648, 2147483647]', function()
      pid(generate_config({
        kp_value = 2147483648
      }))
    end)
  end)

  it('should assert if kp_bits is not in range', function()
    should_fail_with('kp_bits=256 must be in [0, 255]', function()
      pid(generate_config({
        kp_bits = 256
      }))
    end)
  end)

  it('should assert if ki_value is not in range', function()
    should_fail_with('ki_value=2147483648 must be in [-2147483648, 2147483647]', function()
      pid(generate_config({
        ki_value = 2147483648
      }))
    end)
  end)

  it('should assert if ki_bits is not in range', function()
    should_fail_with('ki_bits=256 must be in [0, 255]', function()
      pid(generate_config({
        ki_bits = 256
      }))
    end)
  end)

  it('should assert if kd_value is not in range', function()
    should_fail_with('kd_value=2147483648 must be in [-2147483648, 2147483647]', function()
      pid(generate_config({
        kd_value = 2147483648
      }))
    end)
  end)

  it('should assert if kd_bits is not in range', function()
    should_fail_with('kd_bits=256 must be in [0, 255]', function()
      pid(generate_config({
        kd_bits = 256
      }))
    end)
  end)

  it('should assert if lower_limit is not in range', function()
    should_fail_with('lower_limit=2147483648 must be in [-2147483648, 2147483647]', function()
      pid(generate_config({
        lower_limit = 2147483648
      }))
    end)
  end)

  it('should assert if upper_limit is not in range', function()
    should_fail_with('upper_limit=2147483648 must be in [-2147483648, 2147483647]', function()
      pid(generate_config({
        upper_limit = 2147483648
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type pid', function()
    local expected = remove_whitespace([[
      structure(
        structure(
         i32(11),
         u8(12)
        ),
        structure(
         i32(13),
         u8(14)
        ),
        structure(
         i32(15),
         u8(16)
        ),
        i32(17),
        i32(18))
      ]])

    local actual = pid({
      kp_value = 11,
      kp_bits = 12,
      ki_value = 13,
      ki_bits = 14,
      kd_value = 15,
      kd_bits = 16,
      lower_limit = 17,
      upper_limit = 18
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('pid'))
  end)

  it('should memoize', function()
    should_memoize_calls(pid, generate_config())
  end)

end)

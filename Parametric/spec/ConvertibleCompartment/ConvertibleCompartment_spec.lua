local ConvertibleCompartment = require 'ConvertibleCompartment/ConvertibleCompartment'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('ConvertibleCompartment', function()
  local convertible_compartment = ConvertibleCompartment(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      cabinet_threshold_in_degfx100 = 2000
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(convertible_compartment, generate_config())
  end)

  it('should assert if cabinet_threshold_in_degfx100 is not in range', function()
    should_fail_with('cabinet_threshold_in_degfx100=1000000 must be in [-32768, 32767]', function()
      convertible_compartment(generate_config({
        cabinet_threshold_in_degfx100 = 1000000
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type convertible_compartment', function()
    local expected = remove_whitespace([[
      structure(
        i16(2000)
      )
    ]])

    local actual = convertible_compartment(generate_config())

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('convertible_compartment'))
  end)

  it('should memoize', function()
    should_memoize_calls(convertible_compartment, generate_config())
  end)

end)

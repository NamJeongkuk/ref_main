local RfidFilterData = require 'FilterData/RfidFilterData'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('RfidFilterData', function()
  local rfid_filter_data = RfidFilterData(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      rfid_filter_update_rate = TypedString('rfid_filter_update_rate', 'rfid_filter_update_rate')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(rfid_filter_data, generate_config())
  end)

  it('should assert if rfid_filter_update_rate is not a string', function()
    should_fail_with('rfid_filter_update_rate must be a typed string with type rfid_filter_update_rate, but is a number', function()
      rfid_filter_data(generate_config({
        rfid_filter_update_rate = 1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for rfid_filter_data', function()
    local expected = remove_whitespace([[
        structure(
          pointer(rfid_filter_update_rate)
        )
      ]])

    local actual = rfid_filter_data({
      rfid_filter_update_rate = TypedString('rfid_filter_update_rate', 'rfid_filter_update_rate')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('rfid_filter_data'))
  end)

  it('should memoize', function()
    should_memoize_calls(rfid_filter_data, generate_config())
  end)
end)

local FilterData = require 'FilterData/FilterData'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('FilterData', function()
  local filter_data = FilterData(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      common_filter_data = TypedString('common_filter_data', 'common_filter_data'),
      rfid_filter_data = TypedString('rfid_filter_data', 'rfid_filter_data')
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(filter_data, generate_config())
  end)

  it('should assert if common_filter_data is not a string', function()
    should_fail_with('common_filter_data must be a typed string with type common_filter_data, but is a number', function()
      filter_data(generate_config({
        common_filter_data = 1
      }))
    end)
  end)

  it('should assert if rfid_filter_data is not a string', function()
    should_fail_with('rfid_filter_data must be a typed string with type rfid_filter_data, but is a number', function()
      filter_data(generate_config({
        rfid_filter_data = 2
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type for filter_data', function()
    local expected = remove_whitespace([[
        structure(
          pointer(common_filter_data),
          pointer(rfid_filter_data)
        )
      ]])

    local actual = filter_data({
      common_filter_data = TypedString('common_filter_data', 'common_filter_data'),
      rfid_filter_data = TypedString('rfid_filter_data', 'rfid_filter_data')
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('filter_data'))
  end)

  it('should memoize', function()
    should_memoize_calls(filter_data, generate_config())
  end)
end)

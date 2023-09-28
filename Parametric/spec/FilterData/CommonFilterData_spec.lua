local CommonFilterData = require 'FilterData/CommonFilterData'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args

describe('CommonFilterData', function()
  local common_filter_data = CommonFilterData(core_mock)
  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      minimum_volume_needed_to_start_filter_life_timer_in_ounces = 0,
      filter_month_in_minutes = 43200
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(common_filter_data, generate_config())
  end)

  it('should assert if minimum_volume_needed_to_start_filter_life_timer_in_ounces is not in range', function()
    should_fail_with('minimum_volume_needed_to_start_filter_life_timer_in_ounces=-1 must be in [0, 65535]', function()
      common_filter_data(generate_config({
        minimum_volume_needed_to_start_filter_life_timer_in_ounces = -1
      }))
    end)
  end)

  it('should assert if filter_month_in_minutes is not in range', function()
    should_fail_with('filter_month_in_minutes=-1 must be in [0, 65535]', function()
      common_filter_data(generate_config({
        filter_month_in_minutes = -1
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type common_filter_data', function()
    local expected = remove_whitespace([[
      structure(
        u16(0),
        u16(43200)
      )
      ]])

    local actual = common_filter_data({
      minimum_volume_needed_to_start_filter_life_timer_in_ounces = 0,
      filter_month_in_minutes = 43200
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('common_filter_data'))
  end)

  it('should memoize', function()
    should_memoize_calls(common_filter_data, generate_config())
  end)
end)

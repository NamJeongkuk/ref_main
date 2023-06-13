local ErdMapping = require 'Bsp.ErdMapping'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString
local ErdListToSortedErdMappings = require 'Bsp.ErdListToSortedErdMappings'

describe('erd_list_to_sorted_erd_mappings', function()
  local erd_list_to_sorted_erd_mappings = ErdListToSortedErdMappings(core_mock)

  it('shuld require a table argument erd_list', function()
    should_fail_with('missing argument erd_list', function()
      erd_list_to_sorted_erd_mappings({})
    end)
    should_fail_with('erd_list must be of type table but is of type string', function()
      erd_list_to_sorted_erd_mappings({
        erd_list = 'hello'
      })
    end)
  end)
end)

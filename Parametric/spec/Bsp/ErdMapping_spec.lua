local ErdMapping = require 'Bsp.ErdMapping'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('ErdMapping', function()
  local erd_mapping = ErdMapping(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      input = 1, output = 2
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(erd_mapping, generate_config())
  end)

  it('should require a u16 input parameter', function()
    should_fail_with('missing argument input', function()
      erd_mapping({
        output = 5
      })
    end)

    should_fail_with('input must be of type number but is of type string', function()
      erd_mapping({
        input = '4',
        output = 5
      })
    end)

    should_fail_with('input=-1 must be in [0, 65535]', function()
      erd_mapping({
        input = -1,
        output = 5
      })
    end)

    should_fail_with('input=1.1 must be an integer', function()
      erd_mapping({
        input = 1.1,
        output = 5
      })
    end)
  end)

  it('should require a u16 output parameter', function()
    should_fail_with('missing argument output', function()
      erd_mapping({
        input = 12
      })
    end)

    should_fail_with('output must be of type number but is of type string', function()
      erd_mapping({
        input = 4,
        output = '5'
      })
    end)

    should_fail_with('output=-5 must be in [0, 65535]', function()
      erd_mapping({
        input = 1,
        output = -5
      })
    end)

    should_fail_with('output=5.5 must be an integer', function()
      erd_mapping({
        input = 1,
        output = 5.5
      })
    end)
  end)
end)

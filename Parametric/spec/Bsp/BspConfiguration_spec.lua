local BspConfiguration = require 'Bsp.BspConfiguration'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('BspConfiguration', function()
  local bsp_configuration = BspConfiguration(core_mock)

  local function generate_config(overrides)
    return require 'lua-common'.table.merge({
      binary_search_configuration = TypedString('binary_search_configuration', 'binary_search_configuration'),
      linear_search_configuration = TypedString('linear_search_configuration', 'linear_search_configuration')
    }, overrides or {})
  end

  it('should require all arguments', function()
      should_require_args(bsp_configuration, generate_config())
  end)

  it('should fail with invalid arguments', function()
    should_fail_with('binary_search_configuration must be a typed string with type binary_search_configuration, but is a string', function()
      bsp_configuration(generate_config({
        binary_search_configuration = "the bee movie"
      }))
    end)

    should_fail_with('linear_search_configuration must be a typed string with type linear_search_configuration, but is a string', function()
      bsp_configuration(generate_config({
        linear_search_configuration = ":among-us-dance:"
      }))
    end)
  end)

    it('should be able to generate a bsp configuration', function()
      local expected = remove_whitespace([[
        structure(
          structure(
            binary_search_configuration
          ),
          structure(
            linear_search_configuration
          )
        )
        ]])

      local actual = bsp_configuration(
        generate_config()
      )

      assert.equals(expected, remove_whitespace(tostring(actual)))
      assert(actual.is_of_type('bsp_configuration'))
    end)
end)

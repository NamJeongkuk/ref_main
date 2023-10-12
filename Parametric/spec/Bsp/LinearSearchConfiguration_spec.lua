local LinearSearchConfiguration = require 'Bsp.LinearSearchConfiguration'
local ErdMapping = require 'Bsp.ErdMapping'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('LinearSearchConfiguration', function()
  local linear_search_configuration = LinearSearchConfiguration(core_mock)
  local erd_mapping = ErdMapping(core_mock)

  it('should require a table argument erd_list', function()
    should_fail_with('missing argument erd_list', function()
      linear_search_configuration({})
    end)

    should_fail_with('erd_list must be of type table but is of type string', function()
      linear_search_configuration({
        erd_list = 'hello'
      })
    end)
  end)

  it('should be able to create an erd map with 0 mappings', function()
    local expected = remove_whitespace([[
        pointer(
          structure(
            null(),
            u16(0),
            u16(0),
            u16(0),
            u16(0)
          )
        )
        ]])

    local actual = linear_search_configuration({
      erd_list = {
      }
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('linear_search_configuration'))
  end)

  it('should be able to create an erd map with multiple mappings', function()
    local expected = remove_whitespace([[
        pointer(
          structure(
            pointer(
              structure(
                structure(
                  u16(1),
                  u16(2)
                ),
                structure(
                  u16(2),
                  u16(3)
                ),
                structure(
                  u16(5),
                  u16(6)
                )
              )
            ),
            u16(3),
            u16(4),
            u16(2),
            u16(0)
          )
        )
        ]])

    local actual = linear_search_configuration({
      erd_list = {
        {
          input = 1, output = 2
        },
        {
          input = 2, output = 3
        },
        {
          input = 5, output = 6
        }
      }
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('linear_search_configuration'))
  end)

  it('should be able to create an erd map with multiple mappings', function()
    local expected = remove_whitespace([[
        pointer(
          structure(
            pointer(
              structure(
                structure(
                  u16(1),
                  u16(2)
                ),
                structure(
                  u16(2),
                  u16(3)
                ),
                structure(
                  u16(5),
                  u16(6)
                )
              )
            ),
            u16(3),
            u16(4),
            u16(2),
            u16(0)
          )
        )
        ]])

    local actual = linear_search_configuration({
      erd_list = {
        {
          input = 2, output = 3
        },
        {
          input = 5, output = 6
        },
        {
          input = 1, output = 2
        }
      }
    })

    assert.equals(expected, remove_whitespace(tostring(actual)))
    assert(actual.is_of_type('linear_search_configuration'))
  end)
end)

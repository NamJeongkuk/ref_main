local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local ErdMapping = require 'Bsp.ErdMapping'

return function(core)
  import(core)
  local generate = memoize(function(config)
    if #config.erd_list > 0 then
      local erd_mapping = ErdMapping(core)
      table.sort(config.erd_list, function(a, b) return a.output < b.output end)

      local erd_mappings = {}
      for i, erd_pair in ipairs(config.erd_list) do
        erd_mappings[i] = erd_mapping(erd_pair)
      end
      return erd_mappings
    else
      return {}
    end
  end)

  return function(config)
    validate_arguments(
      config,
      {
        erd_list = { constraint.array_elements({ constraint.type('table') }) }
      }
    )
    return generate(config)
  end
end

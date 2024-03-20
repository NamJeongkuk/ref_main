local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'damper_steps' },
      structure(
        u16(config.steps_to_open),
        u16(config.steps_to_close)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        steps_to_open = { constraint.u16 },
        steps_to_close = { constraint.u16 },
      })
    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'filter_data' },
      structure(
        pointer(config.common_filter_data),
        pointer(config.rfid_filter_data)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        common_filter_data = { constraint.typed_string('common_filter_data') },
        rfid_filter_data = { constraint.typed_string('rfid_filter_data') }
      }
    )
    return generate(config)
  end
end

local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize

return function(core)
  import(core)
  local generate = memoize(function(config)
    return TypedString(
      { 'rfid_filter_data' },
      structure(
        pointer(config.rfid_filter_update_rate),
        u16(config.filter_month_in_minutes)
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
         rfid_filter_update_rate = { constraint.typed_string('rfid_filter_update_rate') },
         filter_month_in_minutes = { constraint.u16 }
      }
    )
    return generate(config)
  end
end

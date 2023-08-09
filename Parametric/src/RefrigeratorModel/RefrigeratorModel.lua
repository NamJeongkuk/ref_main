local import = require 'lua-common'.utilities.import
local constraint = require 'lua-common'.utilities.constraint
local validate_arguments = require 'lua-common'.utilities.validate_arguments
local TypedString = require 'lua-common'.utilities.TypedString
local memoize = require 'lua-common'.util.memoize
local refrigerator_model_type = require 'RefrigeratorModel.RefrigeratorModelType'
local major_configuration_type = require 'RefrigeratorModel.MajorConfigurationType'
local enum = require 'lua-common'.utilities.enum

return function(core)
  import(core)

  local generate = memoize(function(config)
    return TypedString(
      { 'refrigerator_model' },
      structure(
        u8(refrigerator_model_type[config.refrigerator_model_type]),
        u8(0), -- Image ID should be uint16_t and this field is deprecated
        u8(major_configuration_type[config.major_configuration_type])
      )
    )
  end)

  return function(config)
    validate_arguments(
      config,
      {
        refrigerator_model_type = { constraint.in_set(enum.keys(refrigerator_model_type)) },
        major_configuration_type = { constraint.in_set(enum.keys(major_configuration_type)) }
      })

    return generate(config)
  end
end

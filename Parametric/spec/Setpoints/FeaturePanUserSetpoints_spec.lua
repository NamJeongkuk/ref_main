local FeaturePanUserSetpoints = require 'Setpoints.FeaturePanUserSetpoints'
local core_mock = require 'lua-parametric-tools-test'.mock.common.core
local should_memoize_calls = require 'lua-common'.util.should_memoize_calls
local remove_whitespace = require 'lua-common'.utilities.remove_whitespace
local should_fail_with = require 'lua-common'.utilities.should_fail_with
local should_require_args = require 'lua-common'.utilities.should_require_args
local TypedString = require 'lua-common'.util.TypedString

describe('FeaturePanUserSetpoints', function()
  local feature_pan_user_setpoints = FeaturePanUserSetpoints(core_mock)

  local function generate(overrides)
    return require 'lua-common'.table.merge({
      setpoints = {
        TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode1'),
        TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode2'),
        TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode3'),
        TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode4'),
        TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode5'),
        TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode6'),
        TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode7')
      }
    }, overrides or {})
  end

  it('should require all arguments', function()
    should_require_args(feature_pan_user_setpoints, generate())
  end)

  it('should fail if more than 7 setpoints', function()
    should_fail_with('#setpoints=8 must be equal to 7', function()
      feature_pan_user_setpoints(generate({
        setpoints = {
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode1'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode2'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode3'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode4'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode5'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode6'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode7'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode8')
        }
      }))
    end)
  end)

  it('should fail if setpoints is not a type feature_pan_user_setpoint', function()
    should_fail_with('setpoints[1] must be a typed string with type feature_pan_user_setpoint, but is a number', function()
      feature_pan_user_setpoints(generate({
        setpoints = {
          -1,
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode2'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode3'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode4'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode5'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode6'),
          TypedString('feature_pan_user_setpoint', 'feature_pan_user_setpoint_mode7')
        }
      }))
    end)
  end)

  it('should generate a typed string with the correct data and type feature_pan_user_setpoints', function()
    local expected = remove_whitespace([[
      structure(
        pointer(
          structure(
            feature_pan_user_setpoint_mode1,
            feature_pan_user_setpoint_mode2,
            feature_pan_user_setpoint_mode3,
            feature_pan_user_setpoint_mode4,
            feature_pan_user_setpoint_mode5,
            feature_pan_user_setpoint_mode6,
            feature_pan_user_setpoint_mode7
          )
        )
      )
    ]])

    local actual = feature_pan_user_setpoints(generate())

    assert(actual.is_of_type('feature_pan_user_setpoints'))
    assert.equals(expected, remove_whitespace(tostring(actual)))
  end)

  it('should memoize', function()
    should_memoize_calls(feature_pan_user_setpoints, generate())
  end)
end)

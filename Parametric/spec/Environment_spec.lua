describe('The Parametric Environment', function()
  local Environment = require 'Environment'

  it('should not blow up when loaded', function()
    Environment('.')
  end)
end)

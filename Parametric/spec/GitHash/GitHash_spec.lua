local git_hash = require 'GitHash/GitHash'

local function mock_io_popen(result)
  return {
    read = function()
      return result
    end,
    close = function()
      return true
    end
  }
end

describe('git_hash', function()
  it('should fill short git hash with zeros when result is less than 4 bytes', function()
    io.popen = function(command)
      return mock_io_popen("6643")
    end
    assert.are.same({ 0x66, 0x43, 0x00, 0x00 }, git_hash.short())
  end)

  it('should return short git hash when result is equal to 4 bytes', function()
    io.popen = function(command)
      return mock_io_popen("6643bf6f")
    end
    assert.are.same({ 0x66, 0x43, 0xbf, 0x6f }, git_hash.short())
  end)

  it('should return just 4 bytes when result is greater than 4 bytes ', function()
    io.popen = function(command)
      return mock_io_popen("1143bf6f22")
    end
    assert.are.same({ 0x11, 0x43, 0xbf, 0x6f }, git_hash.short())
  end)

  it('should return zero as short git hash when result is not number', function()
    io.popen =  function(command)
      return mock_io_popen("blah")
    end
    assert.are.same({ 0, 0, 0, 0 }, git_hash.short())
  end)
end)

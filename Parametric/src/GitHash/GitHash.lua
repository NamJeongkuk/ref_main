local GitHash =  {}

local function run_command(command)
  local handle = io.popen(command)
  local result = handle:read("*a")
  handle:close()
  return result
end

function GitHash.short()
  local hash_string = run_command("cd Parametric 2>/dev/null; git rev-parse --short=8 HEAD")
  local hash_array = {}
  for i = 1, 8, 2 do
    local hex_pair = string.sub(hash_string, i, i+1)
    table.insert(hash_array, tonumber(hex_pair, 16) or 0)
  end
  return #hash_array == 0 and {0,0,0,0} or hash_array
end

return GitHash

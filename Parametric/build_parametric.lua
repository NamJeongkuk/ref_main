local function extract_path(filename)
  return filename:match("(.*[/\\])") or './'
end

local my_path = extract_path(arg[0])
package.path = package.path .. ';' ..
  my_path .. 'lib/lua-parametric-tools/?.lua;' ..
  my_path .. 'lib/lua-common/?.lua;' ..
  my_path .. 'src/?.lua;'

local remove_tail_calls = require 'lua-common'.util.remove_tail_calls
local env = (require 'Environment')(extract_path(arg[1]))

local loaded_file, err = loadfile(arg[1], 't', env)

if loaded_file then
  loaded_file = remove_tail_calls(loaded_file)
  print(loaded_file())
else
  error(err)
end

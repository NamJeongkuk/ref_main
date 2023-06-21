local function extract_path(require_path)
  return require_path:match('@(.*[/\\])') or './'
end

local previous_path = package.path

local previous_cache = {}
for k, v in pairs(package.loaded) do
  previous_cache[k] = v
  package.loaded[k] = nil
end

local my_path = extract_path(debug.getinfo(1).source)
package.path = my_path .. '/src/?.lua;' .. package.path
package.path = my_path .. '/lib/?/?.lua;' .. package.path

local exports = {
  rockhopper_modify_winning_vote_erds = require 'rockhopper_modify_winning_vote_erds'
}

if not package.loaded.busted then
  for k in pairs(package.loaded) do
    package.loaded[k] = nil
  end
end

for k, v in pairs(previous_cache) do
  package.loaded[k] = v
end

package.path = previous_path

return exports

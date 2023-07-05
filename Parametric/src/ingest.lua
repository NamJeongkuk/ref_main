--[[
Generates a tree data structure representing the calls made in the parametric data and their arguments.
This tree can be traversed to provide a summary of the parametric data.
]]

return function(path)
  local global = {}

  local function tostringify(t)
    if (getmetatable(t) or {}).__tostring then
      return tostring(t)
    end

    if type(t) == 'table' then
      local newTable = {}
      for k, v in pairs(t) do
        local newIndex = type(k) == 'number' and k - 1 or k
        newTable[newIndex] = tostringify(v)
      end
      return newTable
    end

    return t
  end

  local function MetaMetatable(base)
    return {
      __call = function(_, v)
        local o = {}
        o[base] = v

        return o
      end,

      __add = function(a, b)
        return tostring(a) .. ' + ' .. tostring(b)
      end,

      __bor = function(a, b)
        return tostring(a) .. ' | ' .. tostring(b)
      end,

      __tostring = function()
        return base
      end,

      __index = function(_, name)
        return setmetatable({}, MetaMetatable(base and (base .. '.' .. name) or name))
      end
    }
  end

  local import = require 'lua-common'.util.EvalInCurrentEnvironment('./')
  global.import = import

  setmetatable(_G, {
    __index = function(_, name)
      if global[name] then
        return global[name]
      end

      return setmetatable({}, MetaMetatable(name))
    end
  })

  local ingested = tostringify(import(path))
  setmetatable(_G, nil)
  return ingested
end

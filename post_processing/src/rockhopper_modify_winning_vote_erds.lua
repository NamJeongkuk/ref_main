local json = require 'lua-json'.json
local io = require 'Parametric.lib.lua-common.lua-common'.io
local json_pretty = require 'lua-pretty-json'

return function(json_string, output_file_path)
  local data = json.decode(json_string)

  for key, value in pairs(data) do
    if key == "erds" then
      for erdIndex, erdTable in pairs(value) do
        for sub, subv in pairs(erdTable) do
          if(sub == "name" and string.sub(subv, string.len(subv) - 14, string.len(subv)) == "_WinningVoteErd") then
            for dataField, dataValue in pairs(data.erds[erdIndex].data) do
              for valueIndex, valueErd in pairs(dataValue) do
                if(valueIndex == "values") then
                  for valueNumber, winningErds in pairs(valueErd) do
                    local length = #string.sub(subv, 0, string.len(subv) - 14)
                    if(not (string.sub(winningErds, 16, length + 15) == string.sub(subv, 0, string.len(subv) - 14) and string.sub(winningErds, string.len(winningErds) - 3, string.len(winningErds)) == "Vote")) then
                      data.erds[erdIndex].data[dataField].values[valueNumber] = nil
                    end
                  end
                end
              end
            end
          end
        end
      end
    end
  end

  local json_data = json.encode(data)
  local formatted_json = json_pretty.stringify(data, nil, 4)

  io.write_to_file(output_file_path, formatted_json)
end

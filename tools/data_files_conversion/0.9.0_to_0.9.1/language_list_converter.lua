-- This module reads a language list file with the format of solarus 0.9.0
-- (languages/languages.dat) and converts it into the format of solarus 0.9.1
-- (languages/languages.lua).

-- The old format (solarus 0.9.0) is an ini file and the new one is a Lua file.

local ini_parser = require("ini_parser")
local language_list_converter = {}

function language_list_converter.convert(quest_path)

  -- Read the 0.9.0 file.
  local input_file = io.open(quest_path .. "/data/languages/languages.dat")
  if input_file == nil then
    error("Cannot open old language list file for reading: " .. error_message)
  end
  local all_groups, all_ids = ini_parser.parse(input_file)
  input_file:close()

  -- Write the 0.9.1 file.
  local output_file = io.open(quest_path .. "/data/languages/languages.dat", "w")

  output_file:write([[
-- This is a Lua language list file for solarus 0.9.1 or greater.
-- This file was converted from the 0.9.0 ini syntax using the script
-- tools/data_files_conversion/0.9.0_to_0.9.1/language_list_converter.lua.

]])

  for _, code in ipairs(all_ids) do

    local group = all_groups[code]
    output_file:write("language{\n")
    output_file:write("  code = \"" .. code .. "\",\n")
    output_file:write("  name = \"" .. group.name .. "\",\n")
    if group.default then
      output_file:write("  default = true\n")
    end
    output_file:write("}\n\n");
  end
  return all_ids
end

return language_list_converter


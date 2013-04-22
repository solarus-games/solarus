-- This module reads a quest properties file with the format of solarus 0.9.2
-- (quest.dat) and converts it into the format of solarus 1.0.0.

-- The old format (solarus 0.9.2) is an ini file and the new one is a Lua file.

local ini_parser = require("ini_parser")
local converter = {}

function converter.convert(quest_path)

  -- Read the 0.9.2 file.
  local input_file = io.open(quest_path .. "/data/quest.dat")
  if input_file == nil then
    error("Cannot open old quest properties file for reading: " .. error_message)
  end
  local all_groups, all_ids = ini_parser.parse(input_file)
  input_file:close()

  -- Write the 1.0.0 file.
  local output_file = io.open(quest_path .. "/data/quest.dat", "w")

  output_file:write([[
-- This is a quest properties file for solarus 1.0.0 or greater.
-- This file was converted from the 0.9.2 ini syntax using the script
-- tools/data_files_conversion/0.9.2_to_1.0.0/quest_properties_converter.lua.
]])

  local group = all_groups.info
  if group ~= nil then
    output_file:write("quest{\n")
    if group.write_dir ~= nil then
      output_file:write("  write_dir = \"" .. group.write_dir .. "\",\n")
    end
    if group.title_bar ~= nil then
      output_file:write("  title_bar = \"" .. group.title_bar .. "\",\n")
    end
    output_file:write("}\n\n");
  end
end

return converter


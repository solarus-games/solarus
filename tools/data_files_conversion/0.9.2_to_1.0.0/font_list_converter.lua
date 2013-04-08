-- This module reads a font list file with the format of solarus 0.9.2
-- (text/fonts.dat) and converts it into the format of solarus 1.0.0
-- (text/fonts.dat).

-- The old format (solarus 0.9.0) is an ini file and the new one is a Lua file.

local ini_parser = require("ini_parser")
local font_list_converter = {}

function font_list_converter.convert(quest_path)

  -- Read the 0.9.2 file.
  local input_file = io.open(quest_path .. "/data/text/fonts.dat")
  local all_groups, all_ids = ini_parser.parse(input_file)
  input_file:close()

  -- Write the 1.0.0 file.
  local output_file = io.open(quest_path .. "/data/text/fonts.dat", "w")

  output_file:write([[
-- This is a Lua fonts list file for solarus 1.0.0 or greater.
-- This file was converted from the 0.9.2 ini syntax using the script
-- tools/data_files_conversion/0.9.2_to_1.0.0/font_list_converter.lua.
]])

  for _, font_id in ipairs(all_ids) do

    local group = all_groups[font_id]
    output_file:write("font{\n")
    output_file:write("  id = \"" .. font_id .. "\",\n")
    output_file:write("  file = \"" .. group.file .. "\",\n")
    if group.size then
      output_file:write("  size = " .. group.size .. ",\n")
    end
    if group.default then
      output_file:write("  default = true\n")
    end
    output_file:write("}\n\n");
  end
end

return font_list_converter



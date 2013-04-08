#!/usr/bin/lua

-- This script reads a font list file with the format of solarus 0.9.2
-- (text/fonts.dat) and converts it into the format of solarus 1.0.0
-- (text/fonts.lua).

-- The old format (solarus 0.9.0) is an ini file and the new one is a Lua file.

require("../ini_parser")

function print_font_list(all_groups, all_ids)

  print([[
-- This is a Lua fonts list file for solarus 1.0.0 or greater.
-- This file was converted from the 0.9.2 ini syntax using the script
-- tools/data_files_conversion/0.9.2_to_1.0.0/convert_font_list.lua.
]])

  for _, font_id in ipairs(all_ids) do

    local group = all_groups[font_id]
    io.write("font{\n")
    io.write("  id = \"" .. font_id .. "\",\n")
    io.write("  file = \"" .. group.file .. "\",\n")
    if group.size then
      io.write("  size = " .. group.size .. ",\n")
    end
    if group.default then
      io.write("  default = true\n")
    end
    io.write("}\n\n");
  end
end

local all_groups, all_ids = ini_parse.parse(io.stdin)

print_font_list(all_groups, all_ids)



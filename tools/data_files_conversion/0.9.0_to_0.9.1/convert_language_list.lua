#!/usr/bin/lua

-- This script reads a language list file with the format of solarus 0.9.0
-- (languages/languages.dat) and converts it into the format of solarus 0.9.1
-- (languages/languages.lua).

-- The old format (solarus 0.9.0) is an ini file and the new one is a Lua file.

dofile("ini_parser.lua")

function print_language_list(all_groups, all_ids)

  print([[
-- This is a Lua language list file for solarus 0.9.1 or greater.
-- This file was converted from the 0.9.0 ini syntax using the script
-- tools/data_files_conversion/0.9.0_to_0.9.1/convert_language_list.lua.
]])

  for _, code in ipairs(all_ids) do

    local group = all_groups[code]
    io.write("language{\n")
    io.write("  code = \"" .. code .. "\",\n")
    io.write("  name = \"" .. group.name .. "\",\n")
    if group.default then
      io.write("  default = true\n")
    end
    io.write("}\n\n");
  end
end

local all_groups, all_ids = ini_parse()

print_language_list(all_groups, all_ids)



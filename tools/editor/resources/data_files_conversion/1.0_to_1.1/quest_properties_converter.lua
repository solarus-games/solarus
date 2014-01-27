-- This module reads a quest properties file with the format of solarus 1.0
-- (quest.dat) and converts it into the format of solarus 1.1.

-- The only difference is that the value of solarus_version changes.
-- quest.dat also adds some new fields about the quest size but they are
-- optional and their defaut value is okay for this conversion.

local converter = {}

function converter.convert(quest_path)

  local write_dir, title_bar

  -- Read the 1.0 file.
  function quest(properties)

    local solarus_version = properties.solarus_version
    if not solarus_version:match("^1.0%.?") then
      error("This is not a Solarus 1.0 quest: the detected version is " .. solarus_version)
    end

    write_dir = properties.write_dir
    title_bar = properties.title_bar
  end

  dofile(quest_path .. "/data/quest.dat")

  -- Write the 1.1 file.
  local output_file = io.open(quest_path .. "/data/quest.dat", "w")

  output_file:write([[
-- This is a quest properties file for Solarus 1.1.
-- This file was converted from Solarus 1.0 using the script
-- tools/data_files_conversion/1.0_to_1.1/quest_properties_converter.lua.
]])

  output_file:write("quest{\n")
  output_file:write("  solarus_version = \"1.1\",\n")
  if write_dir ~= nil then
    output_file:write("  write_dir = \"" .. write_dir .. "\",\n")
  end
  if title_bar ~= nil then
    output_file:write("  title_bar = \"" .. title_bar .. "\",\n")
  end
  output_file:write("}\n\n");
  output_file:close()

end

return converter


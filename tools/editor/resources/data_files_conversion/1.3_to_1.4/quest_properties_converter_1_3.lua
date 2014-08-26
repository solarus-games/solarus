-- This module reads a quest properties file with the format of Solarus 1.3
-- (quest.dat) and converts it into the format of Solarus 1.4.

-- The only difference is that the value of solarus_version changes.

local converter = {}

function converter.convert(quest_path)

  local properties

  -- Read the old file.
  function quest(p)

    if not p.solarus_version:match("^1.3%.?") then
      error("This is not a Solarus 1.3 quest: the detected version is " .. p.solarus_version)
    end

    properties = p
  end

  dofile(quest_path .. "/data/quest.dat")

  -- Write the new file.
  local output_file = io.open(quest_path .. "/data/quest.dat", "w")

  output_file:write("quest{\n")
  output_file:write("  solarus_version = \"1.4\",\n")
  if properties.write_dir ~= nil then
    output_file:write("  write_dir = \"" .. properties.write_dir .. "\",\n")
  end
  if properties.title_bar ~= nil then
    output_file:write("  title_bar = \"" .. properties.title_bar .. "\",\n")
  end
  if properties.normal_quest_size ~= nil then
    output_file:write("  normal_quest_size = \"" .. properties.normal_quest_size .. "\",\n")
  end
  if properties.min_quest_size ~= nil then
    output_file:write("  min_quest_size = \"" .. properties.min_quest_size .. "\",\n")
  end
  if properties.max_quest_size ~= nil then
    output_file:write("  max_quest_size = \"" .. properties.max_quest_size .. "\",\n")
  end

  output_file:write("}\n\n");
  output_file:close()

end

return converter


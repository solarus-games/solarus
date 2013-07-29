-- Solarus 1.0 to 1.1.
-- Slight change in the tileset syntax: the ground value "water_top_right" was
-- replaced by "wall_top_right_water" because it was very misleading
-- (and the same thing fot the other 3 directions).

local converter = {}

function converter.convert(quest_path, tileset_id)

  local input_file_name = quest_path .. "/data/tilesets/" .. tileset_id .. ".dat"
  local input_file, error_message = io.open(input_file_name)
  if input_file == nil then
    error("Cannot open old tileset file for reading: " .. error_message)
  end

  -- Do the substitution.
  local text = input_file:read("*all")  -- Read the whole file.
  text = text:gsub("water_([a-z_]*)", "wall_%1_water")
  input_file:close()

  local output_file_name = input_file_name
  local output_file, error_message = io.open(output_file_name, "w")
  if output_file == nil then
    error("Cannot open new tileset file for writing: " .. error_message)
  end

  output_file:write(text)
  output_file:close()
end

return converter


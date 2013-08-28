-- Solarus 1.0 to 1.1.
-- Sprite data files change to a more readable syntax.
-- The sprite features remain the same.

local converter = {}

function converter.convert(quest_path, sprite_id)

  local input_file_name = quest_path .. "/data/sprites/" .. sprite_id .. ".dat"
  local input_file, error_message = io.open(input_file_name)
  if input_file == nil then
    error("Cannot open old sprite file for reading: " .. error_message)
  end

  -- Do the changes.
  local text = input_file:read("*all")  -- Read the whole file.
  input_file:close()

  --[[ TODO
  local output_file_name = input_file_name
  local output_file, error_message = io.open(output_file_name, "w")
  if output_file == nil then
    error("Cannot open new sprite file for writing: " .. error_message)
  end

  for line in text:gmatch("[^\n\r]+") do
  end

  output_file:close()
  --]]
end

return converter


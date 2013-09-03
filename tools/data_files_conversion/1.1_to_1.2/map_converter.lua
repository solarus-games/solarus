-- Solarus 1.1 to 1.2.
-- Change in the map syntax: shop_item becomes shop_treasure.

local converter = {}

function converter.convert(quest_path, map_id)

  local input_file_name = quest_path .. "/data/maps/" .. map_id .. ".dat"
  local input_file, error_message = io.open(input_file_name)
  if input_file == nil then
    error("Cannot open old map file for reading: " .. error_message)
  end

  -- Do the substitution.
  local text = input_file:read("*all")  -- Read the whole file.
  text = text:gsub("shop_item{", "shop_treasure{")
  input_file:close()

  local output_file_name = input_file_name
  local output_file, error_message = io.open(output_file_name, "w")
  if output_file == nil then
    error("Cannot open new map file for writing: " .. error_message)
  end

  output_file:write(text)
  output_file:close()
end

return converter


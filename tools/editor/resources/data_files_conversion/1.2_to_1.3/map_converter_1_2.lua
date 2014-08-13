-- Solarus 1.1 to 1.2.
-- Changes in the map syntax
-- - shop_item becomes shop_treasure.
-- - Destructibles are now customizable and no longer have hardcoded subtypes.
-- - Teletransporter now take a string for the transition property.

local converter = {}

local function switch_subtype_replace(before, subtype_line, after)

  local replacement
  if subtype_line == "  subtype = \"0\"," then
    -- Walkable, invisible.
    replacement = [[
  subtype = "walkable",
  sound = "switch",]]

  elseif subtype_line == "  subtype = \"1\"," then
    -- Walkable, visible.
    replacement = [[
  subtype = "walkable",
  sprite = "entities/switch",
  sound = "switch",]]

  elseif subtype_line == "  subtype = \"2\"," then
    -- Arrow target.
    replacement = [[
  subtype = "arrow_target",
  sound = "switch",]]

  elseif subtype_line == "  subtype = \"3\"," then
    -- Solid switch.
    replacement = [[
  subtype = "solid",
  sprite = "entities/solid_switch",
  sound = "switch",]]

  else
    error("Unrecognized switch subtype line: " .. subtype_line)
  end

  return before .. replacement .. after
end

function converter.convert(quest_path, map_id)

  local input_file_name = quest_path .. "/data/maps/" .. map_id .. ".dat"
  local input_file, error_message = io.open(input_file_name)
  if input_file == nil then
    error("Cannot open old map file for reading: " .. error_message)
  end

  local text = input_file:read("*a")  -- Read the whole file.

  -- Replace switch subtypes.
  text = text:gsub("(switch{\n[^}]*)(  subtype = \"[0-3]\",)([^}]*})", switch_subtype_replace)

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


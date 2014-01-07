-- Solarus 1.1 to 1.2.
-- Changes in the map syntax
-- - shop_item becomes shop_treasure.
-- - Destructibles are now customizable and no longer have hardcoded subtypes.
-- - Teletransporter now take a string for the transition property.

local converter = {}

local destructible_subtype_replacements = {

  ['  subtype = "pot",'] = [[
  sprite = "entities/pot",
  destruction_sound = "stone",
  damage_on_enemies = 2,]],

  ['  subtype = "bush",'] = [[
  sprite = "entities/bush",
  destruction_sound = "bush",
  weight = 1,
  can_be_cut = true,]],

  ['  subtype = "white_stone",'] = [[
  sprite = "entities/stone_small_white",
  destruction_sound = "stone",
  weight = 1,
  damage_on_enemies = 2,]],

  ['  subtype = "black_stone",'] = [[
  sprite = "entities/stone_small_black",
  destruction_sound = "stone",
  weight = 2,
  damage_on_enemies = 4,]],

  ['  subtype = "grass",'] = [[
  sprite = "entities/grass",
  destruction_sound = "bush",
  weight = -1,
  can_be_cut = true,
  ground = "grass",]],

  ['  subtype = "bomb_flower",'] = [[
  sprite = "entities/bomb_flower",
  destruction_sound = "bush",
  weight = 1,
  can_be_cut = true,
  can_explode = true,
  can_regenerate = true,]],

}

local teletransporter_transition_replacements = {
  ['  transition = 0,'] = '  transition = "immediate",',
  ['  transition = 1,'] = '  transition = "fade",',
  ['  transition = 2,'] = '  transition = "scrolling",',
}


function converter.convert(quest_path, map_id)

  local input_file_name = quest_path .. "/data/maps/" .. map_id .. ".dat"
  local input_file, error_message = io.open(input_file_name)
  if input_file == nil then
    error("Cannot open old map file for reading: " .. error_message)
  end

  local text = input_file:read("*a")  -- Read the whole file.

  -- Replace shop_item by shop_treasure.
  text = text:gsub("shop_item{", "shop_treasure{")

  -- Replace destructible subtypes.
  text = text:gsub("(  subtype = \"[a-z_]+\",)", destructible_subtype_replacements)

  -- Replace teletransporter transitions.
  text = text:gsub("(  transition = [0-2],)", teletransporter_transition_replacements)

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


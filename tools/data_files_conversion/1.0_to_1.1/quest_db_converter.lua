-- This module reads a quest resource list file with the format of solarus 1.0
-- (project_db.dat) and converts it into the format of solarus 1.1.

-- The new syntax is more readable.
-- Also, languages are now clearly a resource and their list is rebuilt from
-- the obsolete language list file languages.dat.
-- And we take the opportunity to add all sprites that we find existing but
-- that were not present in the file.

local converter = {}

local resource_type_names = {
  [0] = "map",
  [1] = "tileset",
  [2] = "music",
  [3] = "sprite",
  [4] = "sound",
  [5] = "item",
  [6] = "enemy",
  [7] = "language",
}

-- List of sprites that might exist and that were not always referenced in
-- project_db.dat of old quests.
--
-- It is time to fix this problem because project_db.dat is meant to be the
-- reliable list of resources. 
-- If sprites exist but are not mentioned in project_db.dat, we add them
-- so that they have a chance to be converted
-- (because yes, the format of sprites also changes in 1.1).
-- We try to find as much sprites as possible and we add them to
-- project_db.dat if they were mot present.
-- The problem is that we can't be sure to find (and therefore convert)
-- all of them.
-- If you have a 1.0 project with a sprite that does not gets converted,
-- because it is not in project_db.dat and not in the following list, you
-- will have to convert it explicitly by using sprite_converter.lua.
-- Sorry for this mess.
local sprites_possibly_unlisted = {

  -- Various entity sprites.
  { id = "entities/block",                  description = "Block" },
  { id = "entities/chest",                  description = "Chest" },
  { id = "entities/conveyor_belt",          description = "Conveyor belt" },
  { id = "entities/crystal",                description = "Crystal" },
  { id = "entities/bomb",                   description = "Bomb" },
  { id = "entities/explosion",              description = "Explosion" },
  { id = "entities/fire",                   description = "Fire" },
  { id = "entities/grass",                  description = "Grass" },
  { id = "entities/bush",                   description = "Bush" },
  { id = "entities/pot",                    description = "Pot" },
  { id = "entities/bomb_flower",            description = "Bomb flower" },
  { id = "entities/arrow",                  description = "Arrow" },
  { id = "entities/hookshot",               description = "Hookshot" },
  { id = "entities/items",                  description = "Items" },
  { id = "entities/shadow",                 description = "Shadow" },
  { id = "entities/star",                   description = "Star" },
  { id = "entities/stone_small_black",      description = "Small black stone" },
  { id = "entities/stone_small_white",      description = "Small white stone" },
  { id = "entities/switch",                 description = "Switch" },
  { id = "entities/solid_switch",           description = "Solid switch" },

  -- Sprites used in the old built-in menus.
  -- Quests are likely to have kept the same sprites.
  { id = "hud/dialog_box_message_end.dat",  description = "Dialog end" },
  { id = "hud/gameover_fade.dat",           description = "HUD: Game-over fade" },
  { id = "hud/action_icon_flip.dat",        description = "HUD: Action icon" },
  { id = "hud/pause_icon_flip.dat",         description = "HUD: Pause icon" },
  { id = "hud/sword_icon_flip.dat",         description = "HUD: Sword icon" },
  { id = "hud/empty_heart.dat",             description = "HUD: Empty heart" },
  { id = "hud/magic_bar.dat",               description = "HUD: Magic bar" },
  { id = "menus/arrow.dat",                 description = "Menu: Arrow" },
  { id = "menus/selection_menu_cursor.dat", description = "Menu: Savegame cursor" },
  { id = "menus/pause_cursor.dat",          description = "Menu: Pause cursor" },
  { id = "menus/pause_save_dialog.dat",     description = "Menu: Pause save dialog" },
  { id = "menus/options_cursor.dat",        description = "Menu: Options cursor" },
  { id = "menus/hero_head.dat",             description = "Menu: Hero head" },
  { id = "menus/hero_point.dat",            description = "Menu: Hero point" },

  -- Hero sprites.
  { id = "hero/tunic1",                     description = "Hero tunic 1" },
  { id = "hero/tunic2",                     description = "Hero tunic 2" },
  { id = "hero/tunic3",                     description = "Hero tunic 3" },
  { id = "hero/sword1",                     description = "Hero sword 1" },
  { id = "hero/sword2",                     description = "Hero sword 2" },
  { id = "hero/sword3",                     description = "Hero sword 3" },
  { id = "hero/sword4",                     description = "Hero sword 4" },
  { id = "hero/sword_stars1",               description = "Hero sword stars 1" },
  { id = "hero/sword_stars2",               description = "Hero sword stars 2" },
  { id = "hero/sword_stars3",               description = "Hero sword stars 3" },
  { id = "hero/sword_stars4",               description = "Hero sword stars 4" },
  { id = "hero/shield1",                    description = "Hero shield 1" },
  { id = "hero/shield2",                    description = "Hero shield 2" },
  { id = "hero/shield3",                    description = "Hero shield 3" },
  { id = "hero/ground1",                    description = "Grass under hero" },
  { id = "hero/ground2",                    description = "Shallow water under hero" },
  { id = "hero/trail",                      description = "Hero trail" },

  -- Enemy sprites.
  -- Most of enemy sprites will actually be determined automatically from the
  -- list of enemies, but we add additional names here because some enemies
  -- have several sprites, and only the most commonly named ones can be guessed.
  -- So we hardcode some sprite names here, hoping we cover all of them.
  { id = "enemies/enemy_killed",            description = "Enemy killed" },
  { id = "enemies/black_knight_soldier_sword",
                                            description = "Enemy: Black knight soldier sword" },
  { id = "enemies/khorneth_left_blade",     description = "Enemy: Khorneth left blade" },
  { id = "enemies/khorneth_right_blade",    description = "Enemy: Khorneth right blade" },
  { id = "enemies/poutroxor_head",          description = "Enemy: Poutroxor head" },
  { id = "enemies/poutroxor_skeleton",      description = "Enemy: Poutroxor skeleton" },

}
local sprites_listed = {}

local function load_quest_db(quest_path, languages)

  languages = languages or {}
  local resources = {}

  -- Create a table for each type of resource.
  for _, resource_type_name in pairs(resource_type_names) do
    resources[resource_type_name] = {}
  end

  local file, error_message = io.open(quest_path .. "/data/project_db.dat")
  if file == nil then
    error("Cannot read project_db.dat file: " .. error_message)
  end

  local line_number = 0
  for line in file:lines() do
    line_number = line_number + 1
    local resource_type_index, resource_id, resource_description =
        line:match("([0-9])\t(.*)\t(.*)")
    if resource_type_index == nil then
      error("Line " .. line_number .. ": Wrong resource type")
    end
    local resource_type_name = resource_type_names[tonumber(resource_type_index)]
    if resource_type_name == nil then
      error("Line " .. line_number .. ": Wrong resource type")
    end

    if resource_id == nil then
      error("Line " .. line_number .. ": Wrong resource id")
    end

    if resource_description == nil then
      error("Line " .. line_number .. ": Wrong resource name")
    end

    if resource_type_name ~= "language" then
      -- Ignore languages mentioned in project_db.dat, they were useless before.

      local resources_of_current_type = resources[resource_type_name]
      resources_of_current_type[#resources_of_current_type + 1] = {
        id = resource_id,
        description = resource_description
      }
    end
  end

  -- Now add the languages coming from the only reliable place
  -- (the old language list file).
  local language_resources = resources["language"]
  for _, language in ipairs(languages) do
    language_resources[#language_resources + 1] = {
      id = language.code,
      description = language.name,
    }
  end

  -- Add the sprites that exist but were not referenced here until now.
  local sprite_resources = resources["sprite"]
  for _, sprite in ipairs(sprite_resources) do
    sprites_listed[sprite.id] = true
  end

  -- Try to guess enemy sprite ids.
  local enemy_resources = resources["enemy"]
  for _, enemy in ipairs(enemy_resources) do
    sprites_possibly_unlisted[#sprites_possibly_unlisted + 1] =
    {
      id = "enemies/" .. enemy.id,
      description = "Enemy: " .. enemy.description,
    }
    sprites_possibly_unlisted[#sprites_possibly_unlisted + 1] =
    {
      -- Lost of enemies had a separated sword sprite in old projects.
      id = "enemies/" .. enemy.id .. "_sword",
      description = "Enemy: " .. enemy.description .. " sword",
    }
  end

  for _, sprite in ipairs(sprites_possibly_unlisted) do
    if not sprites_listed[sprite.id] and
        io.open(quest_path .. "/data/sprites/" .. sprite.id .. ".dat") then
      -- The sprite file exists but is not listed in project_db.dat: add it.
      sprite_resources[#sprite_resources + 1] = sprite
    end
  end

  return resources
end

function converter.convert(quest_path, languages)

  -- Read project_db.dat.
  local resources = load_quest_db(quest_path, languages)

  -- Write the 1.1 file.
  local output_file = io.open(quest_path .. "/data/project_db.dat", "w")

  for i = 0, 7 do
    local resource_type_name = resource_type_names[i]
    local resource = resources[resource_type_name]

    for _, element in ipairs(resource) do
      output_file:write(resource_type_name)
      output_file:write("{ id = \"")
      output_file:write(element.id)
      output_file:write("\", description = \"")
      output_file:write(element.description)
      output_file:write("\" }\n")
    end
    output_file:write("\n");
  end
  output_file:close()

  return resources
end

return converter


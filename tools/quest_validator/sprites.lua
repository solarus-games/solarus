-- This module checks the sprites of a quest.
-- Only one function does the work: validator.check().

-- TODO check the existence of mandatory sprites.

local validator = {}

local report = require("report")

-- Checks an individual sprite.
-- Returns a table animation_id -> animation.
local function check_sprite(quest_path, sprite_id)

  print("Checking sprite '" .. sprite_id .. "'")

  local animations = {}

  local env = {}
  function env.animation(properties)

    if properties.name == nil then
      error("Animation without name", 2)
    end

    animations[properties.name] = properties

    -- TODO check properties
  end

  local file = quest_path .. "sprites/" .. sprite_id .. ".dat"
  local chunk, error = loadfile(file)
  if chunk == nil then
    report.error("Error in sprite '" .. sprite_id .. "': " .. error)
  else
    setfenv(chunk, env)
    local success, error = pcall(chunk)

    if not success then
      report.error("Error in sprite '" .. sprite_id .. "': " .. error)
    end
  end

  return animations
end

-- Checks all sprites.
-- Returns a table of all sprite sheets: sprite_id -> animations.
function validator.check(quest_path, resources)

  print("Checking sprites")

  local sprite_resource = resources.sprite
  local sprites = {}

  for _, sprite_element in ipairs(sprite_resource) do
    local sprite_id = sprite_element.id
    local sprite = check_sprite(quest_path, sprite_id)
    sprites[sprite_id] = sprite
  end

  return sprites
end

return validator


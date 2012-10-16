#!/usr/bin/lua

-- This script reads a map data file with the format of solarus 0.9.2
-- and converts it into the format of solarus 1.0.0.

-- The old format (solarus 0.9.2) is a text file with a specific syntax.
-- The new format (solarus 1.0.0) is a Lua data file.

-- This table describes the old syntax.
local entity_syntaxes = {

  -- entity type id -> entity line syntax
  [0] = {
    entity_type_name = "tile",
    -- token index -> token name and token type (default int)
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "pattern" },
  },

  [1] = {
    entity_type_name = "destination",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "sprite", token_type = "string", nil_value = "_none" },
  },

  [2] = {
    entity_type_name = "teletransporter",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "sprite", token_type = "string", nil_value = "_none" },
    { token_name = "sound", token_type = "string", nil_value = "_none" },
    { token_name = "transition", token_type = "int" },
    { token_name = "destination_map", token_type = "string" },
    { token_name = "destination", token_type = "string" },
  },

  [3] = {
    entity_type_name = "pickable",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "treasure_name", token_type = "string" },
    { token_name = "treasure_variant" },
    { token_name = "treasure_savegame_variable", nil_value = -1 },
  },

  [4] = {
    entity_type_name = "destructible",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "subtype" },
    { token_name = "treasure_name", token_type = "string" },
    { token_name = "treasure_variant" },
    { token_name = "treasure_savegame_variable", nil_value = -1 },
  },

  [5] = {
    entity_type_name = "chest",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "is_big_chest", token_type = "boolean" },
    { token_name = "treasure_name", token_type = "string" },
    { token_name = "treasure_variant" },
    { token_name = "treasure_savegame_variable", nil_value = -1 },
  },

  [6] = {
    entity_type_name = "jumper",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "jump_length" },
  },

  [7] = {
    entity_type_name = "enemy",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "breed", token_type = "string" },
    { token_name = "rank", token_type = "int" },
    { token_name = "savegame_variable", nil_value = -1 },
    { token_name = "treasure_name", token_type = "string" },
    { token_name = "treasure_variant" },
    { token_name = "treasure_savegame_variable", nil_value = -1 },
  },

  [8] = {
    entity_type_name = "npc",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "subtype" },
    { token_name = "sprite", token_type = "string", nil_value = "_none" },
    { token_name = "behavior", token_type = "string" },
  },

  [9] = {
    entity_type_name = "block",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "sprite", token_type = "string", nil_value = "_none" },
    { token_name = "pushable", token_type = "boolean" },
    { token_name = "pullable", token_type = "boolean" },
    { token_name = "maximum_moves" },
  },

  [10] = {
    entity_type_name = "dynamic_tile",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "pattern" },
    { token_name = "enabled_at_start", token_type = "boolean" },
  },

  [11] = {
    entity_type_name = "switch",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "subtype" },
    { token_name = "needs_block", token_type = "boolean" },
    { token_name = "inactivate_when_leaving", token_type = "boolean" },
  },

  [12] = {
    entity_type_name = "wall",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "stops_hero", token_type = "boolean" },
    { token_name = "stops_enemies", token_type = "boolean" },
    { token_name = "stops_npcs", token_type = "boolean" },
    { token_name = "stops_blocks", token_type = "boolean" },
  },

  [13] = {
    entity_type_name = "sensor",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
  },

  [14] = {
    entity_type_name = "crystal",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
  },

  [15] = {
    entity_type_name = "crystal_block",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "subtype" },
  },

  [16] = {
    entity_type_name = "shop_item",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "treasure_name", token_type = "string" },
    { token_name = "treasure_variant" },
    { token_name = "treasure_savegame_variable", nil_value = -1 },
    { token_name = "price" },
    { token_name = "dialog", token_type = "string" },
  },

  [17] = {
    entity_type_name = "conveyor_belt",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "direction" },
  },

  [18] = {
    entity_type_name = "door",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "subtype" },
    { token_name = "savegame_variable" },
  },

  [19] = {
    entity_type_name = "stairs",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "subtype" },
  },

}

local world_names = {
  [-1] = "inside_world",
  [0] = "outside_world"
}
for i = 1, 20 do
  world_names[i] = "dungeon_" .. i
end

local floor_names = {
  [-100] = nil,  -- No floor.
  [-99] = "unknown",
}
for i = -16, 15 do
  floor_names[i] = tostring(i)
end

function parse_metadata(line)

  local metadata = {}
  local width, height, world, floor, x, y, small_keys_variable, tileset, music =
    line:match("^([0-9]+)%s+([0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([a-zA-Z0-9_]+)%s+([a-zA-Z0-9_]+)")

  if width == nil then
    error("Line 1: Invalid map metadata")
  end

  metadata.x = tonumber(x)
  metadata.y = tonumber(y)
  metadata.width = tonumber(width)
  metadata.height = tonumber(height)
  metadata.world = world_names[tonumber(world)]
  metadata.floor = floor_names[tonumber(floor)]

  if small_keys_variable ~= -1 then
    metadata.small_keys_variable = tonumber(small_keys_variable)
  end

  metadata.tileset = tileset

  if music ~= "none" then
    metadata.music = music
  end

  return metadata
end

function print_metadata(metadata)

  io.write("properties{\n")
  io.write("  x = " .. metadata.x .. ",\n")
  io.write("  y = " .. metadata.y .. ",\n")
  io.write("  width = " .. metadata.width .. ",\n")
  io.write("  height = " .. metadata.height .. ",\n")
  io.write("  world = \"" .. metadata.world .. "\",\n")
  if metadata.floor then
    io.write("  floor = " .. metadata.floor .. ",\n")
  end
  if metadata.small_keys_variable ~= -1 then
    io.write("  small_keys_variable = " .. metadata.small_keys_variable .. ",\n")
  end
  io.write("  tileset = " .. metadata.tileset .. ",\n")
  if metadata.music ~= nil then
    io.write("  music = " .. metadata.music .. ",\n")
  end
  io.write("}\n\n")
end

function parse_entity(line, line_number)

  local entity = {}
  local entity_type_id = nil
  local i = 1
  local syntax
  for token in line:gmatch("([^%s]+)%s*") do
    if entity_type_id == nil then
      -- First token: entity type.
      entity_type_id = tonumber(token)
      if entity_type_id == nil then
        error("Line " .. line_number .. ": Invalid entity type id")
      end
      syntax = entity_syntaxes[entity_type_id]
      entity.entity_type_name = syntax.entity_type_name
    else
      local token_type = syntax[i].token_type 
      local value
      if token_type == "string" then
        value = token
      elseif token_type == "boolean" then
        if token ~= "0" then
          value = true
        else
          value = false
        end
      elseif token_type == nil then  -- Integer.
        value = tonumber(token)
        if value == nil then
          error("Line " .. line_number .. ": Number expected for token '" ..
              syntax[i].token_name .. "'")
        end
      else
        error("Line " .. line_number .. ": Unknown type '" .. token_type ..
            " for token '" .. syntax[i].token_name .. "'")
      end
      if value == syntax[i].nil_value then
        value = nil
      end

      entity[i] = {
        key = syntax[i].token_name,
        value = value
      }

      i = i + 1
    end
  end

  return entity
end

function print_entity(entity)

  io.write("" .. entity.entity_type_name .. "{\n")
  for i, v in ipairs(entity) do
    local value
    if type(v.value) == "string" then
      value = "\"" .. v.value .. "\""
    elseif type(v.value) == "boolean" then
      if v.value then
	value = "true"
      else
	value = "false"
      end
    else  -- Integer or nil.
      value = v.value
    end
    if value ~= nil then
      io.write("  " .. v.key .. " = " .. value .. ",\n")
    end
  end
  io.write("}\n\n")
end

local file = io.stdin
local lines = file:lines()

-- The first line is the map metadata.
local first_line = lines()
if first_line == nil then
  error("Empty map data file")
end
local metadata = parse_metadata(first_line)
print_metadata(metadata)


local line_number = 1
for line in file:lines() do

  line_number = line_number + 1
  local entity = parse_entity(line, line_number)
  print_entity(entity)
end


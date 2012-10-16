#!/usr/bin/lua

-- This script reads a map data file with the format of solarus 0.9.2
-- and converts it into the format of solarus 1.0.0.

-- The old format (solarus 0.9.2) is a text file with a specific syntax.
-- The new format (solarus 1.0.0) is a Lua data file.

local entity_type_names = {
  [0] = "tile",
  [1] = "destination",
  [2] = "teletransporter",
  [3] = "pickable",
  [4] = "destructible",
  [5] = "chest",
  [6] = "jump_sensor",
  [7] = "enemy",
  [8] = "npc",
  [9] = "block",
  [10] = "dynamic_tile",
  [11] = "switch",
  [12] = "wall",
  [13] = "sensor",
  [14] = "crystal",
  [15] = "crystal_block",
  [16] = "shop_item",
  [17] = "conveyor_belt",
  [18] = "door",
  [19] = "stairs",
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

local entity_types = {

  [0] = {
    type_name = "destination"
    tokens = { "layer", "x", "y", "width", "height", "pattern_id" }
  }
}

function parse_metadata(line)

  local metadata = {}
  local width, height, world, floor, x, y, small_keys_variable, tileset_id, music_id =
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

  metadata.tileset_id = tileset_id

  if music_id ~= "none" then
    metadata.music_id = music_id
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
  io.write("  tileset_id = " .. metadata.tileset_id .. ",\n")
  if metadata.music_id ~= nil then
    io.write("  music_id = " .. metadata.music_id .. ",\n")
  end
  io.write("}\n")
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

  -- Parse this entity.
  line_number = line_number + 1

  -- Print the entity.
end


#!/usr/bin/lua

-- This script updates all data files of a solarus 0.9.2 quest
-- into the format of solarus 1.0.0.
-- Usage: lua update_quest.lua path/to/your_quest

local quest_path = ...
if quest_path == nil then
  print("Usage: lua update_quest.lua path/to/your_quest")
  os.exit()
end

print("Updating your quest " .. quest_path .. " from Solarus 0.9.2 to Solarus 1.0.0")
print("It is recommended to backup your quest files before.")

-- Convert the font list file.
print("  Converting the font list file...")
local font_list_converter = require("font_list_converter")
font_list_converter.convert(quest_path)

-- Read the project_db.dat file to get the list of maps and tilesets.
print("  Reading the list of maps and tilesets...")
local quest_db_loader = require("quest_db_loader")
local resources = quest_db_loader.load_quest_db(quest_path)

-- Convert tilesets.
print("  Converting tilesets...")
local tileset_converter = require("tileset_converter")
for _, resource in pairs(resources["tileset"]) do
  print("    Tileset " .. resource.id .. " (" .. resource.human_name .. ")")
  tileset_converter.convert(quest_path, resource.id)
end
print("  All tilesets were converted.")

-- Convert maps.
print("  Converting maps...")
local map_converter = require("map_converter")
for _, resource in pairs(resources["map"]) do
  print("    Map " .. resource.id .. " (" .. resource.human_name .. ")")
  map_converter.convert(quest_path, resource.id)
end
print("  All maps were converted.")

print("Update successful!")


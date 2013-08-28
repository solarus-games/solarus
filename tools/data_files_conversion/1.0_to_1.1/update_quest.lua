#!/usr/bin/lua

-- This script updates all data files of a solarus 1.0 quest
-- into the format of solarus 1.1.
-- Usage: lua update_quest.lua path/to/your_quest

local function write_info(message)

  io.write(message, "\n")
  io.flush()
end

local quest_path = ...
if quest_path == nil then
  write_info("Usage: lua update_quest.lua path/to/your_quest")
  os.exit()
end

write_info("Updating your quest " .. quest_path .. " from Solarus 1.0 to Solarus 1.1.")
write_info("It is recommended to backup your quest files before.")

-- Remove the language list file languages/languages.dat.
write_info("  Removing the obsolete language list file...")
local language_list_converter = require("language_list_converter")
local languages = language_list_converter.convert(quest_path)

-- Convert the quest properties file quest.dat.
write_info("  Converting the quest properties file...")
local quest_properties_converter = require("quest_properties_converter")
quest_properties_converter.convert(quest_path)

-- Convert the resource list file project_db.dat.
write_info("  Converting the resource list file...")
local quest_db_converter = require("quest_db_converter")
local resources = quest_db_converter.convert(quest_path, languages)

-- Convert tilesets.
write_info("  Converting tilesets...")
local tileset_converter = require("tileset_converter")
for _, resource in pairs(resources["tileset"]) do
  print("    Tileset " .. resource.id .. " (" .. resource.description .. ")")
  tileset_converter.convert(quest_path, resource.id)
end
print("  All tilesets were converted.")

-- Convert sprites.
write_info("  Converting sprites...")
local sprite_converter = require("sprite_converter")
for _, resource in pairs(resources["sprite"]) do
  print("    Sprite " .. resource.id .. " (" .. resource.description .. ")")
  sprite_converter.convert(quest_path, resource.id)
end
print("  All sprites were converted.")

write_info("Update successful!")


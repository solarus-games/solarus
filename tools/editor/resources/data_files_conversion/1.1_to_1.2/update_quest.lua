#!/usr/bin/lua

-- This script updates all data files of a solarus 1.1 quest
-- into the format of solarus 1.2.
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

write_info("Updating your quest " .. quest_path .. " from Solarus 1.1 to Solarus 1.2.")
write_info("It is recommended to backup your quest files before.")

-- Convert the quest properties file quest.dat.
write_info("  Converting the quest properties file...")
local quest_properties_converter = require("quest_properties_converter")
quest_properties_converter.convert(quest_path)

-- Read the resource list file project_db.dat.
write_info("  Reading the list of resources...")
local quest_db_converter = require("quest_db_converter")
local resources = quest_db_converter.convert(quest_path)

-- Convert maps.
write_info("  Converting maps...")
local map_converter = require("map_converter")
for _, resource in pairs(resources["map"]) do
  write_info("    Map " .. resource.id .. " (" .. resource.description .. ")")
  map_converter.convert(quest_path, resource.id)
end
write_info("  All maps were converted.")

write_info("Update successful!")


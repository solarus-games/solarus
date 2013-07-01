#!/usr/bin/lua

-- This script updates all data files of a solarus 1.0 quest
-- into the format of solarus 1.1.
-- Usage: lua update_quest.lua path/to/your_quest

local quest_path = ...
if quest_path == nil then
  print("Usage: lua update_quest.lua path/to/your_quest")
  os.exit()
end

print("Updating your quest " .. quest_path .. " from Solarus 1.0 to Solarus 1.1.")
print("It is recommended to backup your quest files before.")

-- Convert the quest properties file.
print("  Converting the quest properties file...")
local quest_properties_converter = require("quest_properties_converter")
quest_properties_converter.convert(quest_path)

-- Convert the resource list file project_db.dat.
print("  Converting the resource list file...")
local quest_db_converter = require("quest_db_converter")
quest_db_converter.convert(quest_path)

print("Update successful!")


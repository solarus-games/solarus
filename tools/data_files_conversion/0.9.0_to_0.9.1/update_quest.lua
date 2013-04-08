#!/usr/bin/lua

-- This script updates all data files of a solarus 0.9.2 quest
-- into the format of solarus 1.0.0.
-- Usage: lua update_quest.lua path/to/your_quest

local quest_path = ...
if quest_path == nil then
  print("Usage: lua update_quest.lua path/to/your_quest")
  os.exit()
end

print("Updating your quest " .. quest_path .. " from Solarus 0.9.0 to Solarus 0.9.1.")
print("It is recommended to backup your quest files before.")

-- Convert the language list file.
local language_list_converter = require("language_list_converter")
print("  Converting the language list file...")
local language_ids = language_list_converter.convert(quest_path)

-- Convert the dialogs.
local dialogs_converter = require("dialogs_converter")
for _, language_id in ipairs(language_ids) do
  print("  Converting the dialogs of language " .. language_id .. "...")
  dialogs_converter.convert(quest_path, language_id)
end

print("Update successful!")


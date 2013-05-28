#!/usr/bin/lua

-- This script converts an individual dialogs file from the format of
-- solarus 0.9.0 into the format of solarus 0.9.1.
-- Usage: lua update_dialogs.lua path/to/your/quest language_id

local quest_path, language_id = ...
if quest_path == nil or language_id == nil then
  print("Usage: lua update_dialogs.lua path/to/your_quest language_id")
  os.exit()
end

local dialogs_converter = require("dialogs_converter")
dialogs_converter.convert(quest_path, language_id)


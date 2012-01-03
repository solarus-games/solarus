#!/usr/bin/lua

-- Checks the validity of the data files of a quest
-- (experimental and very incomplete for now)
-- Usage: ./validator.lua path/to/your/quest


dofile("languages.lua")

if #arg ~= 1 then
  print("Usage: " .. arg[0] .. " path/to/your/quest")
  os.exit()
end

local quest_dir = arg[1] .. "/data"

-- temporary
local fr = load_dialogs(quest_dir .. "/languages/fr/text/dialogs.lua")
--local en = load_dialogs(quest_dir .. "/languages/en/text/dialogs.lua")
--check_dialogs_translation(en, fr)

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
check_languages(quest_dir)


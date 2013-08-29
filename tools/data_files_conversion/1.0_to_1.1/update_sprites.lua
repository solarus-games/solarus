#!/usr/bin/lua

-- This script converts individual sprites from the format of Solarus 1.0
-- into the format of Solarus 1.1.
-- Usage: lua update_sprites.lua path/to/your/quest sprite_id ...

local sprite_converter = require("sprite_converter")

local args = { ... }
local quest_path = args[1]
local sprite_1_id = args[2]
if quest_path == nil or sprite_1_id == nil then
  print("Usage: lua update_sprites.lua path/to/your_quest sprite_id ...")
  os.exit()
end

print("  Converting sprites...")
for i = 2, #args do
  print("    Sprite " .. args[i])
  sprite_converter.convert(quest_path, args[i])
end
print("  All sprites were converted.")


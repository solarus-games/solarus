#!/usr/bin/lua

-- This script converts an individual tileset from the format of solarus 0.9.2
-- into the format of solarus 1.0.0.
-- Usage: lua update_tileset.lua path/to/your/quest tileset_id

local tileset_converter = require("tileset_converter")

local quest_path, tileset_id = ...
if quest_path == nil or tileset_id == nil then
  print("Usage: lua update_tileset.lua path/to/your_quest tileset_id")
  os.exit()
end

local tileset_converter = require("tileset_converter")
tileset_converter.convert(quest_path, tileset_id)


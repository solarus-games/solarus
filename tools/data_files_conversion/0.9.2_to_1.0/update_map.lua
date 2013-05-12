#!/usr/bin/lua

-- This script converts an individual map from the format of solarus 0.9.2
-- into the format of solarus 1.0.0.
-- Usage: lua update_map.lua path/to/your/quest map_id

local map_converter = require("map_converter")

local quest_path, map_id = ...
if quest_path == nil or map_id == nil then
  print("Usage: lua update_map.lua path/to/your_quest map_id")
  os.exit()
end

local map_converter = require("map_converter")
map_converter.convert(quest_path, map_id)


#!/usr/bin/lua

-- Checks the validity of the data files of a quest
-- (experimental and very incomplete for now)
-- Usage: ./validator.lua path/to/your/quest

local report = require("report")
local resource_list_validator = require("resource_list")
local language_validator = require("languages")
local sprite_validator = require("sprites")
local item_validator = require("items")

if #arg ~= 1 then
  print("Usage: " .. arg[0] .. " path/to/your/quest")
  os.exit()
end

print("*** Starting validation of quest " .. arg[1] .. " ***")

local quest_path = arg[1] .. "/data/"

local resources = resource_list_validator.check(quest_path)
local dialogs = language_validator.check(quest_path, resources)
local sprites = sprite_validator.check(quest_path, resources)
item_validator.check(quest_path, resources, dialogs, sprites)

print("*** Validation completed with " .. report.get_num_warnings() ..
    " warning(s) and " .. report.get_num_errors() .. " error(s). ***")


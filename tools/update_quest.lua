#!/usr/bin/lua

-- This script updates all data files of a solarus quest
-- into the most recent format.
-- Usage: lua update_quest.lua path/to/your_quest

local solarus_formats = {
  "1.0",
  "1.1",
}

local quest_path = ...
if quest_path == nil then
  print("Usage: lua update_quest.lua path/to/your_quest")
  os.exit()
end

-- Determines and returns the format of a quest.
local function get_quest_format(quest_path)

  local quest_format = nil

  -- Read the quest properties file.
  function quest(properties)

    quest_format = properties.solarus_version
  end

  local quest_file = loadfile(quest_path .. "/data/quest.dat")
  if quest_file == nil then
    error("No quest was found in '" .. quest_path .. "'")
  end
  quest_file()

  if quest_format ~= nil then
    quest_format = quest_format:match("^([0-9]+%.[0-9]+)%.?")
  end
  return quest_format
end

-- Makes an upgrade step from the current format to the next one.
local function update_step(quest_path, old_format, new_format)

  local old_package_path = package.path
  local step_directory = old_format .. "_to_" .. new_format
  package.path = "data_files_conversion/" .. step_directory .. "/?.lua;" .. package.path 
  local script_path = "data_files_conversion/" .. step_directory .. "/update_quest.lua"
  local update_script = loadfile(script_path)
  if update_script == nil then
    error("Cannot find script '" .. script_path .. "'")
  end
  update_script(quest_path)
  package.path = old_package_path
end

local quest_format = get_quest_format(quest_path)

if quest_format == nil then
  error("Failed to determine the format of this quest")
end

local solarus_format = solarus_formats[#solarus_formats]
print("Format of the quest: " .. quest_format)
print("Latest Solarus format: " .. solarus_format)

if quest_format == solarus_format then
  -- Nothing to do.
  print("This quest is already up-to-date.")
else
  -- Update to do.
  local started = false
  for _, format in ipairs(solarus_formats) do
    if started then
      print("====== Step " .. quest_format .. " to " .. format .. " ======")
      update_step(quest_path, quest_format, format)
      print("====== Step done ======")
      print()
      quest_format = format
    elseif format == quest_format then
      started = true
    end
  end
  print("All updates were successful!")
end


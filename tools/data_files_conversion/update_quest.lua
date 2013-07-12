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

local function get_quest_format(quest_path)

  local quest_format = nil

  -- Read the quest properties file.
  function quest(properties)

    quest_format = properties.solarus_version
  end

  dofile(quest_path .. "/data/quest.dat")

  if quest_format ~= nil then
    quest_format = quest_format:match("^([0-9]+%.[0-9]+)%.?")
  end
  return quest_format
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
      print("Updating to " .. format)
      -- TODO
    elseif format == quest_format then
      started = true
    end
  end
  print("All updates were successful!")
end


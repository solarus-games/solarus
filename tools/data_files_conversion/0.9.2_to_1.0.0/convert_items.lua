#!/usr/bin/lua

-- This script reads a items.dat data file with the format of solarus 0.9.2
-- and converts it into the format of solarus 1.0.0.

-- The old format (solarus 0.9.2) is an ini file.
-- The new format (solarus 1.0.0) is a Lua data file.

dofile("../0.9.0_to_0.9.1/ini_parser.lua")

function print_items(all_groups, all_ids)

  print([[
-- This is a Lua items list file for solarus 1.0.0 or greater.
-- This file was converted from the 0.9.2 ini syntax using the script
-- tools/data_files_conversion/0.9.2_to_1.0.0/convert_items.lua.
]])

  for _, item_name in ipairs(all_ids) do

    local group = all_groups[item_name]
    io.write("item{\n")
    io.write("  name = \"" .. item_name .. "\",\n")
    if group.savegame_variable ~= nil and group.savegame_variable ~= "-1" then
      io.write("  savegame_variable = \"i" .. group.savegame_variable .. "\",\n")
    end
    if group.nb_variants ~= nil then
      io.write("  nb_variants = " .. group.nb_variants .. ",\n")
    end
    if group.initial_variant ~= nil then
      io.write("  initial_variant = " .. group.initial_variant .. ",\n")
    end
    if group.can_be_assigned ~= nil then
      io.write("  can_be_assigned = " .. group.can_be_assigned .. ",\n")
    end
    if group.counter ~= nil and group.counter ~= "-1" then
      io.write("  counter = \"i" .. group.counter .. "\",\n")
    end
    if group.limit ~= nil then
      io.write("  limit = " .. group.limit .. ",\n")
    end
    if group.limit_for_counter ~= nil then
      io.write("  limit_for_counter = \"" .. group.limit_for_counter .. "\",\n")
    end
    if group.changes_counter ~= nil then
      io.write("  changes_counter = \"" .. group.changes_counter .. "\",\n")
    end
    if group.amount ~= nil then
      io.write("  amount = " .. group.amount .. ",\n")
    end
    if group.probability ~= nil then
      io.write("  probability = " .. group.probability .. ",\n")
    end
    if group.shadow ~= nil then
      io.write("  shadow = \"" .. group.shadow .. "\",\n")
    end
    if group.can_disappear ~= nil then
      io.write("  can_disappear = " .. group.can_disappear .. ",\n")
    end
    if group.brandish_when_pickabled == nil then
      io.write("  brandish_when_picked = true,\n")
    end
    if group.sound_when_picked ~= nil then
      io.write("  sound_when_picked = \"" .. group.sound_when_picked .. "\",\n")
    end
    if group.sound_when_brandished ~= nil then
      io.write("  sound_when_brandished = \"" .. group.sound_when_brandished .. "\",\n")
    end

    local nb_variants = group.nb_variants or 1
    for i = 1, nb_variants do
      if group["amount_" .. i] ~= nil then
	io.write("  amount_" .. i .. " = " .. group["amount_" .. i] .. ",\n")
      end
      if group["probability_" .. i] ~= nil then
	io.write("  probability_" .. i .. " = " .. group["probability_" .. i] .. ",\n")
      end
    end

    io.write("}\n\n");
  end
end

local all_groups, all_ids = ini_parse()
print_items(all_groups, all_ids)


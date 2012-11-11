#!/usr/bin/lua

-- This script reads a items.dat data file with the format of solarus 0.9.2
-- and converts it into the format of solarus 1.0.0.

-- The old format (solarus 0.9.2) is an ini file.
-- The new format (solarus 1.0.0) is a Lua data file.

-- TODO implement this converter.
-- For now it only converts the savegame variables to valid identifiers in order
-- to be compatible with the new savegame format.

local file = io.stdin
local lines = file:lines()

for line in file:lines() do

  line = line:gsub("savegame_variable = ", "savegame_variable = i")
  io.write(line .. "\n")
end


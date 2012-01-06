#!/usr/bin/lua

-- This script reads a language list file with the format of solarus 0.9.0
-- (languages/languages.dat) and converts it into the format of solarus 0.9.1
-- (languages/languages.lua).

-- The old format (solarus 0.9.0) is an ini file and the new one is a Lua file.

dofile("ini_parser.lua")

local all_groups, all_ids = ini_parse()


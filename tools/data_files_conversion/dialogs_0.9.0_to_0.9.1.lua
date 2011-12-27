#!/usr/bin/lua

-- This script reads a dialog file with the format of solarus 0.9.0
-- and converts it into the format of solarus 0.9.1.

-- The old format (solarus 0.9.0) is an ini file.
-- In a given dialog, all groups of 3 lines are explicitely separated.
-- Example:

-- [wise_man]
-- #       |-----------------------|
-- line1 = "I can feel courage in"
-- line2 = "you, $v."
-- line3 = "Do you think you can"
-- next = wise_man.2
-- skip = current
-- 
-- [wise_man.2]
-- #       |-----------------------|
-- line1 = "protect the Princess?"
-- line2 = "Definitely!"
-- line3 = "No way"
-- question = true
-- next = wise_man.thanks
-- next2 = wise_man.insisting
-- 
-- [wise_man.insisting]
-- #       |-----------------------|
-- line1 = "$v, I beg you."
-- line2 = "You are the only"
-- line3 = "one able to protect"
-- next = wise_man.insisting.2
-- 
-- [wise_man.insisting.2]
-- #       |-----------------------|
-- line1 = "our kingdom..."
-- line2 = "Sure"
-- line3 = "No way"
-- question = true
-- next = wise_man.thanks
-- next2 = wise_man.insisting
-- 
-- [wise_man.thanks]
-- #       |-----------------------|
-- line1 = "I knew I could count on"
-- line2 = "your immense bravery."

-- In the new format, the dialog file is in Lua.
-- (Lua was designed to represent data files and not only usual scripts.)
-- The separation in groups of 3 lines is now made by the engine.
-- Dialog writers and translators do not need to handle this separation
-- manually.
-- Example:

-- Dialog{
--   id = "wise_man",
--   skip = "current",
--   question = true,
--   next = "wise_man.thanks"
--   next2 = "wise_man.insisting"
--   text = [[
-- I can feel courage in
-- you, $v.
-- Do you think you can
-- protect the Princess?
-- Definitely!
-- No way
--   ]],
-- }
-- -----------------------
-- 
-- Dialog{
--   id = "wise_man.insisting",
--   question = true,
--   next = "wise_man.thanks"
--   next2 = "wise_man.insisting"
--   text = [[
--     $v, I beg you.
--     You are the only
--     one able to protect
--     our kingdom...
--     Sure
--     No way
--   ]],
-- }
-- -----------------------
-- 
-- Dialog{
--   id = "wise_man.thanks",
--   text = [[
--     I knew I could count on
--     your immense bravery.
--   ]],
-- }
-- -----------------------

for line in io.lines() do

  -- TODO
end


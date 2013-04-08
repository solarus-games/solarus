#!/usr/bin/lua

-- This module reads a dialog file with the format of solarus 0.9.0
-- and converts it into the format of solarus 0.9.1.

-- The old format (solarus 0.9.0) is an ini file.
-- In a given dialog, all groups of 3 lines (messages) are explicitely
-- separated.
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
-- The separation in groups of 3 lines (messages) is now made by the engine.
-- Dialog writers and translators no longer need to handle this separation
-- manually.
-- Example:

-- dialog{
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
-- dialog{
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
-- dialog{
--   id = "wise_man.thanks",
--   text = [[
--     I knew I could count on
--     your immense bravery.
--   ]],
-- }
-- -----------------------

local dialogs_converter = {}

-- Reads the ini file and returns a table with all messages and
-- an array with all their ids
local function parse_ini_dialogs(input_file)

  local all_ids = {}      -- array of all message ids
  local all_messages = {} -- table of all messages
  local message = nil     -- current message
  local line_number = 0

  for line in input_file:lines() do

    line_number = line_number + 1

    -- skip empty lines
    if not line:find("^[ \t]*$") then

      local id = line:match("^ *%[(.*)%]")
      local icon = tonumber(line:match("^ *icon *= *([-0-9]*)"))
      local skip = line:match("^ *skip *= *([a-z]*)")
      local next = line:match("^ *next *= *(.*)")
      local next2 = line:match("^ *next2 *= *(.*)")
      local question = line:match("^ *question *= *([a-z]*)")
      local line1 = line:match("^ *line1 *= *\"(.*)\"")
      local line2 = line:match("^ *line2 *= *\"(.*)\"")
      local line3 = line:match("^ *line3 *= *\"(.*)\"")
      local comment = line:match("^ *# *(.*)$")

      if id then
        -- new message
        if message ~= nil and message.id ~= nil then
          if all_messages[message.id] then
            error("Duplicate message '" .. message.id .. "'")
          end
          -- finish the previous message
          all_messages[message.id] = message 
          all_ids[#all_ids + 1] = message.id
        end
        -- create a new one
        message = {}
        message.id = id
      elseif icon then
        -- line with an icon number
        message.icon = tonumber(icon)
      elseif skip then
        -- line with the skip property ("none", "current" or "all")
        message.skip = skip
      elseif next then
        -- line with the id of the next message
        message.next = next
      elseif next2 then
        -- line with the id of an alternative next message (for a question)
        message.next2 = next2
      elseif question then
        -- line with the question boolean property
        message.question = (question == "true")
      elseif line1 then
        -- first line of text
        message.line1 = line1
      elseif line2 then
        -- second line of text
        message.line2 = line2
      elseif line3 then
        -- third line of text
        message.line3 = line3
      elseif comment then
        -- comments
        if message ~= nil and not comment:find("^[-| \t]*|[-| \t]*$") then -- skip line size comments
          if message.line1 or message.line2 or message.line3 then
            if message.comments_after == nil then
              message.comments_after = {}
            end
            message.comments_after[#message.comments_after + 1] = comment
          else
            if message.comments == nil then
              message.comments = {}
            end
            message.comments[#message.comments + 1] = comment
          end
        end
      else
        error("Line " .. line_number .. ": This is not an ini file: '" .. line .. "'")
      end
    end
  end

  if message == nil then
    error("No messages were found. Is this a solarus 0.9.0 dialog file?")
  end

  all_messages[message.id] = message
  all_ids[#all_ids + 1] = message.id

  return all_messages, all_ids
end

-- For each message, builds the list of messages such that this message is an
-- immediate successor.
function find_previous(all_messages)

  for k, v in pairs(all_messages) do
    if v.next and v.next ~= "_unknown" then
      if all_messages[v.next] == nil then
        error("Line " .. line_number .. ": This is not an ini file: '" .. line .. "'")
      end
      if all_messages[v.next].previous == nil then
        all_messages[v.next].previous = {}
      end
      local pred = all_messages[v.next].previous
      pred[#pred + 1] = k
    end
    if v.next2 and v.next ~= "_unknown" then
      if all_messages[v.next2] == nil then
        error("Error in message '" .. k .. "': no such second next message '"
            .. v.next2 .. "'")
      end
      if all_messages[v.next2].previous == nil then
        all_messages[v.next2].previous = {}
      end
      local pred = all_messages[v.next2].previous
      pred[#pred + 1] = k
    end
  end
end

-- Print verbose messages about what can_be_merged() does.
local function debug_can_be_merged(all_messages, id)

  local message = all_messages[id]
  if message == nil then
    return
  end

  print("Can we merge message '" .. message.id .. "'?")
  if message.previous == nil then
    print("No: this message has no predecessors, it's the first of a dialog")
  elseif #message.previous ~= 1 then
    print("No: this message has multiple predecessors (" .. #message.previous
        .. ")")
  elseif all_messages[message.previous[1]].question then
    print("No: the predecessor is a question")
  elseif not (message.id:find("%.[0-9][0-9]*$")
      or message.id:find("%.[0-9][0-9]*bis$")
      or message.id:find("%.[0-9][0-9]*ter$")) then
    print("No: this message does not have a typical sequence suffix")
  elseif message.icon ~= nil
      and message.icon ~= all_messages[message.previous[1]].icon then
    print("No: this message changes the icon")
  elseif message.skip ~= nil
      and message.skip ~= all_messages[message.previous[1]].skip then
    print("No: this message changes the skip property")
  else
    print("Yes")
  end
end

-- Determines whether a message can be merged with its predecessor
-- into a sequence.
-- Note that if the dialogs don't follow the 0.9.0 conventions for successive
-- messages, there is no perfect way to be sure
local function can_be_merged(all_messages, id)

  -- debug_can_be_merged(all_messages, id)

  local message = all_messages[id]

  -- if the message has only one predecessor, and has an id with has a typical
  -- sequence suffix, and does not change icon or skip,
  -- then we allow the merge
  return message ~= nil
      and message.previous ~= nil
      and #message.previous == 1
      and not all_messages[message.previous[1]].question
      and (message.id:find("%.[0-9][0-9]*$")
          or message.id:find("%.[0-9][0-9]*bis$")
          or message.id:find("%.[0-9][0-9]*ter$"))
      and (message.icon == nil
          or (message.icon == all_messages[message.previous[1]].icon))
      and (message.skip == nil
          or (message.skip == all_messages[message.previous[1]].skip))
  -- we assume that scripts and maps never refer to messages having both a
  -- sequence suffix and a unique predecessor
  -- (if they do, a dialog won't be created for them as they are merged!)
end

-- Merges individual messages into dialogs (sequences of messages) and
-- returns them.
local function merge_messages(all_messages, all_ids)

  -- group messages together
  local sequences = {}

  for _, id in ipairs(all_ids) do
    if not can_be_merged(all_messages, id) then
      -- it's the beginning of a sequence
      local m = all_messages[id]
      sequences[id] = {}
      sequences[id][1] = m

      while can_be_merged(all_messages, m.next) do
        -- the current message is not a question and its successor
        -- can be merged with it
        m = all_messages[m.next]
        sequences[id][#sequences[id] + 1] = m
      end
    end
  end

  -- convert message sequences into dialogs
  local dialogs = {}
  for _, id in ipairs(all_ids) do
    if sequences[id] then
      local s = sequences[id]
      local dialog = {}
      dialogs[#dialogs + 1] = dialog
      dialog.id = s[1].id
      dialog.icon = s[1].icon
      dialog.skip = s[1].skip
      dialog.question = s[#s].question
      dialog.next = s[#s].next
      dialog.next2 = s[#s].next2
      dialog.comments = s[1].comments
      dialog.comments_after = s[#s].comments_after
      dialog.text = ""
      for _, m in pairs(s) do
        local blanks = ""
        if not m.line1 then
          m.line1 = ""
        end
        if not m.line2 then
          m.line2 = ""
        end
        if not m.line3 then
          m.line3 = ""
        end
        dialog.text = dialog.text
            .. m.line1 .. "\n"
            .. m.line2 .. "\n"
            .. m.line3 .. "\n"
      end

      -- remove final empty lines
      dialog.text = dialog.text:gsub("\n\n*$", "\n")
    end
  end

  return dialogs
end

-- Prints the dialogs on an output file in the 0.9.1 syntax
local function print_dialogs(output_file, dialogs) 

  output_file:write([[
-- This is a Lua dialog file for solarus 0.9.1 or greater.
-- This dialog file was converted from the 0.9.0 ini syntax using the script
-- tools/data_files_conversion/0.9.0_to_0.9.1/dialogs_converter.lua.

]])

  for _, v in ipairs(dialogs) do

    if v.comments then
      for _, c in ipairs(v.comments) do
        output_file:write("-- " .. c .. "\n")
      end
    end
    output_file:write("dialog{\n")
    output_file:write("  id = \"" .. v.id .. "\",\n")
    if v.icon then
      output_file:write("  icon = " .. v.icon .. ",\n")
    end
    if v.skip then
      output_file:write("  skip = \"" .. v.skip .. "\",\n")
    end
    if v.question then
      output_file:write("  question = true,\n")
    end
    if v.next then
      output_file:write("  next = \"" .. v.next .. "\",\n")
    end
    if v.next2 then
      output_file:write("  next2 = \"" .. v.next2 .. "\",\n")
    end
    if not v.text then
      error("Dialog '" .. v.id .. "' has no text")
    end
    output_file:write("  text = [[\n")
    output_file:write(v.text)
    output_file:write("]]\n")
    output_file:write("}\n")
    if v.icon then
      output_file:write("--------------------\n\n")
    else
      output_file:write("-----------------------\n\n")
    end
    if v.comments_after then
      for _, c in ipairs(v.comments_after) do
        output_file:write("-- " .. c .. "\n")
      end
    end
  end
end

function dialogs_converter.convert(quest_path, language_id)

  local input_file, error_message = io.open(
      quest_path .. "/data/languages/" .. language_id .. "/text/dialogs.dat")
  if input_file == nil then
    error("Cannot open old dialog file for reading: " .. error_message)
  end

  local all_messages, all_ids = parse_ini_dialogs(input_file)
  find_previous(all_messages)
  local dialogs = merge_messages(all_messages, all_ids)

  local output_file = io.open(
      quest_path .. "/data/languages/" .. language_id .. "/text/dialogs.dat",
      "w")
  if input_file == nil then
    error("Cannot open new dialog file for writing: " .. error_message)
  end
  print_dialogs(output_file, dialogs)
end

return dialogs_converter


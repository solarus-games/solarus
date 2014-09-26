-- This module checks the language files of a quest.
-- Only one function does the work: validator.check().

-- TODO: currently, the dialogs and their translations are checked, but
-- the strings and the language-specific images also have to be checked.

local validator = {}

local report = require("report")

-- Arbitrary non-string key used to add meta-info to each dialog.
local file_line_key = {}

-- Checks the validity of the specified dialog set.
local function check_dialogs(dialogs)

  for id, dialog in pairs(dialogs) do

    local file, line = dialog[file_line_key].file, dialog[file_line_key].line
    if dialog.text == nil then
      report.error("Dialog '" .. id .. "': Missing text", file, line)
    elseif type(dialog.text) ~= "string"
      and type(dialog.text) ~= "number"
      and type(dialog.text) ~= "boolean" then
      report.error("Dialog '" .. id .. "': Wrong field 'text': expected string, number or boolean, got " .. type(dialog.text), 
          file, line)
    end

    -- TODO check each field and its type.

  end
end

-- Returns a table of all dialogs from the specified language.
local function load_dialogs(quest_path, language_id)

  local file = quest_path .. "languages/" .. language_id
      .. "/text/dialogs.dat"

  local env = {}
  local dialogs = {}
  function env.dialog(properties)

    if properties.id == nil then
      error("Dialog without id", 2)
    elseif type(properties.id) ~= "string" then
      error("Dialog id must be a string", 2)
    else
      dialogs[properties.id] = properties
      local file, line = report.get_file_line(2)
      dialogs[properties.id][file_line_key] = {}
      dialogs[properties.id][file_line_key].file = file
      dialogs[properties.id][file_line_key].line = line
    end
  end

  local chunk, error = loadfile(file)
  if chunk == nil then
    report.error("Error in dialogs of language '" .. language_id .. "': " .. error)
  else
    setfenv(chunk, env)
    local success, error = pcall(chunk)

    if not success then
      report.error("Error in dialogs of language '" .. language_id .. "': " .. error)
    else
      check_dialogs(dialogs)
    end
  end

  return dialogs
end

-- Checks that a dialog set matches a reference dialog set.
-- More precisely, checks that all dialogs of th reference set also exist
-- in the new set and that no other dialog exist. Also checks that the
-- properties are the same (except for the text).
local function inspect_dialogs_translation(
  translated_language_id, translated_dialogs, original_language_id, original_dialogs)

  for id, original in pairs(original_dialogs) do

    -- Any original dialog should also exist in the translated set.
    local translated = translated_dialogs[id]
    if translated == nil then
      report.error("Missing dialog '" .. id .. "' in language '" ..
          translated_language_id .. "' (it exists in language '" ..
          original_language_id .. "')",
          original_dialogs[file_line_key].file,
          original_dialogs[file_line_key].line
      )
    else

      -- The properties should be the same
      -- TODO check any field (no more built-in properties except id and text)
      for _, k in ipairs{"skip", "icon", "question", "next", "next2"} do
	if translated[k] ~= original[k] then
	  report.error("Dialog '" .. id .. "': property '" .. k ..
	      "' in language '" .. translated_language_id ..
              "' differs from language '" .. original_language_id .. "'",
	      translated[file_line_key].file,
              translated[file_line_key].line
          )
	end
      end
    end
  end

  for id, translated in pairs(translated_dialogs) do

    -- A translated dialog should exist in the original set
    if original_dialogs[id] == nil then
      report.error("Unexpected dialog '" .. id .. "' in language '" ..
          translated_language_id .. "' (does not exist in language '" ..
	  original_language_id .. "')",
          translated[file_line_key].file,
          translated[file_line_key].line
      )
    end
  end
end

-- Loads and checks all language files.
-- Returns a table language -> dialogs.
function validator.check(quest_path, resources)

  print("Checking languages")

  local language_resource = resources.language
  local first_id, first_dialogs
  local dialogs_by_language = {}

  for _, element in ipairs(language_resource) do

    local id = element.id
    local description = element.description

    print("Checking language '" .. id .. "'")

    local dialogs = load_dialogs(quest_path, id)
    dialogs.file = file

    if not first_id then
      first_id = id
      first_dialogs = dialogs
    else
      print("Checking the validity of '" .. id ..
          "' with respect to reference language '" .. first_id .. "'")
      inspect_dialogs_translation(id, dialogs, first_id, first_dialogs)
    end
    dialogs_by_language[id] = dialogs
  end

  return dialogs_by_language
end

return validator


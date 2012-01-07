-- This module checks the language files of a quest.
-- Only one public function does the work: check_languages(quest_dir).

-- TODO: currently, the dialogs and their translations are checked, but
-- the strings and the language-specific images also have to be checked.

-- Loads and checks all language files.
function check_languages(quest_dir)

  print("Reading the list of languages")

  local file = quest_dir .. "/languages/languages.lua"
  local languages, language_codes = load_language_list(file)
  local first_code, first_dialogs

  for _, code in ipairs(language_codes) do
    local language = languages[code]

    print("Checking language '" .. code .. "'")

    local file = quest_dir .. "/languages/" .. code
        .. "/text/dialogs.lua"
    local dialogs = load_dialogs(file)
    dialogs.file = file

    if not first_code then
      first_code = code
      first_dialogs = dialogs
    else
      print("Checking the validity of '" .. code ..
          "' translation from original language '" .. first_code .. "'")
      inspect_dialogs_translation(dialogs, first_dialogs)
    end
  end
end

-- Returns the current file and line of a stack frame (depth 0 means you)
function get_file_line(depth)

  local info = debug.getinfo(depth + 1, "Sl")
  local file = info.source:sub(2)
  local line = info.currentline
  return file, line
end

-- Prints an error message with optional file name and line number.
function print_error(error, file, line)

  io.stderr:write("[ERROR] ")
  if file then
    io.stderr:write(file .. ":")
    if line then
      io.stderr:write(line .. ":")
    end
  end
  io.stderr:write(" " .. error .. "\n")
end

-- Returns a table of all languages and an array of all language codes
function load_language_list(language_list_file)

  local codes = {}
  local languages = {}
  function language(properties)

    local file, line = get_file_line(2)

    if properties.code == nil then
      print_error("Language without code", file, line)
    else
      languages[properties.code] = properties
      codes[#codes + 1] = properties.code
    end
  end

  local res, error = pcall(loadfile(language_list_file))
  if error then
    print_error(error, language_list_file)
  end

  return languages, codes
end

-- Returns a table of all dialogs from the specified dialog file
function load_dialogs(dialog_file)

  local dialogs = {}
  function dialog(properties)

    local file, line = get_file_line(2)

    if properties.id == nil then
      print_error("Dialog without id", file, line)
    else
      dialogs[properties.id] = properties
      dialogs[properties.id].file = file
      dialogs[properties.id].line = line
    end
  end

  local res, error = pcall(loadfile(dialog_file))
  if error then
    print_error(error, dialog_file)
  else
    inspect_dialogs(dialogs)
  end

  return dialogs
end

-- Checks the validity of the specified dialog set.
function inspect_dialogs(dialogs)

  for id, dialog in pairs(dialogs) do

    -- TODO check each field and its type
    -- also check that there is no unknown field

    -- check the successors
    if dialog.next
        and dialog.next ~= "_unknown"
        and not dialogs[dialog.next] then
      print_error("Cannot find successor '" .. dialog.next .. "'",
          dialog.file, dialog.line)
    end

    if dialog.next2
        and dialog.next2 ~= "_unknown"
        and not dialogs[dialog.next2] then
      print_error("Cannot find alternative successor '" .. dialog.next2 .. "'",
          dialog.file, dialog.line)
    end
  end
end

-- Checks that a dialog set matches an original dialog set.
-- More precisely, checks that all dialogs of the original set also exist
-- in the new set and that no other dialog exist. Also checks that the
-- properties are the same (except for the text).
function inspect_dialogs_translation(translated_dialogs, original_dialogs)

  for id, original in pairs(original_dialogs) do

    -- an original dialog should exist in the translated set
    local translated = translated_dialogs[id]
    if translated == nil then
      print_error("Dialog '" .. id .. "' is not translated", translated_dialogs.file)
    else

      -- the properties should be the same
      for _, k in ipairs{"skip", "icon", "question", "next", "next2"} do
	if translated[k] ~= original[k] then
	  print_error("Dialog '" .. id .. "': '" .. k
	  .. "' is different from the original",
	      translated.file, translated.line)
	end
      end
    end
  end

  for id, translated in pairs(translated_dialogs) do

    -- a translated dialog should exist in the original set
    if original_dialogs[id] == nil then
      print_error("Unknown translated dialog '" .. id
          .. "' (no such dialog in the original file)",
	  translated.file, translated.line)
    end
  end
end


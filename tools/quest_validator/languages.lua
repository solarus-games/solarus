-- Prints an error message with optional file name and line number.
function print_error(error, file, line)

  if file then
    io.write(file .. ":")
    if line then
      io.write(line .. ": ")
    end
  end
  io.write(error .. "\n")
end

-- Returns a table of all dialogs from the specified dialog file
function load_dialogs(dialog_file)

  if dialog_file == nil then
    print_error("No dialog file specified")
  end

  local dialogs = {}
  function dialog(properties)

    local info = debug.getinfo(2, "Sl")
    local file = info.source
    local line = info.currentline

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
    check_dialogs(dialogs)
  end

  return dialogs
end

-- Checks the validity of the specified dialog set.
function check_dialogs(dialogs)

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
function check_dialogs_translation(translated_dialogs, original_dialogs)

  for id, original in pairs(original_dialogs) do

    -- an original dialog should exist in the translated set
    local translated = translated_dialogs[id]
    if translated == nil then
      print_error("Dialog '" .. id .. "' is not translated",
          translated.file)
    else

      -- the properties should be the same
      for _, k in ipairs{"skip", "icon", "question", "next", "next2"} do
	if translated[k] ~= original[k] then
	  print_error("Dialog '" .. id .. "': '" .. k
	  .. "' is different from the original",
	      translated[k].file, translated[k].line)
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


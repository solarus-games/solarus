
-- Returns a table of all dialogs from the specified dialog file
function load_dialogs(dialog_file)

  if dialog_file == nil then
    error("No diaog file specified")
  end

  local dialogs = {}
  function dialog(properties)
    dialogs[properties.id] = properties
  end

  dofile(dialog_file)

  return dialogs
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
      error("Dialog '" .. id .. "' is not translated")
    end

    -- the properties should be the same
    for _, k in ipairs{"skip", "icon", "question", "next", "next2"} do
      if translated[k] ~= original[k] then
        error("Dialog '" .. id .. ": '" .. k
	    .. "' is different from the original")
      end
    end
  end

  for id, translated in pairs(translated_dialogs) do

    -- a translated dialog should exist in the original set
    if original_dialogs[id] == nil then
      error("Unknown translated dialog '" .. id .. "' (no such dialog in the original file")
    end
  end
end


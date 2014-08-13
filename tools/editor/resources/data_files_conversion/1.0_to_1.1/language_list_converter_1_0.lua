-- This module reads a language list file with the format of Solarus 1.0
-- (languages/languages.dat), stores its content and deletes it since
-- it is obsolete in Solarus 1.1.

local converter = {}

function converter.convert(quest_path)

  local languages = {}

  -- Read the 1.0 file.
  function language(properties)

    local code = properties.code or error("Missing code field")
    local name = properties.name or error("Missing name field")
    languages[#languages + 1] = {
      code = code,
      name = name
    }
  end

  local input_file = quest_path .. "/data/languages/languages.dat"
  dofile(input_file)

  -- Delete the file.
  local success, error_message = os.remove(input_file)
  if not success then
    error("Could not delete the obsolete language list file: " .. error_message)
  end

  -- Return the language list so that other converters can use it.
  return languages
end

return converter


-- This module reads a strings.dat file with the format of Solarus 1.1
-- and converts it into the format of Solarus 1.2.
-- The new syntax is more readable.

local converter = {}

local function load_strings(quest_path, language)

  local elements = {}

  local file, error_message = io.open(
      quest_path .. "/data/languages/" .. language .. "/text/strings.dat")
  if file == nil then
    error("Cannot read strings.dat file for language '" .. language
        .. "': " .. error_message)
  end

  local line_number = 0
  for line in file:lines() do
    line_number = line_number + 1
    -- Skip empty lines and comments.
    if #line > 0 and line:sub(1, 1) ~= "#" then
      local key, value = line:match("^([^ \t]*)[ \t]*([^\t]*)$")
      if key == nil or value == nil then
        error("Line " .. line_number .. ": Syntax error")
      end
      elements[#elements + 1] = {
        key = key,
        value = value,
      }
    end
  end

  file:close()

  return elements
end

function converter.convert(quest_path, language)

  -- Read strings.dat.
  local strings = load_strings(quest_path, language)

  -- Write the 1.2 file.
  local output_file, error_message = io.open(
      quest_path .. "/data/languages/" .. language .. "/text/strings.dat", "w")
  if output_file == nil then
    error("Cannot write strings.dat file for language '" .. language
        .. "': " .. error_message)
  end

  for _, element in ipairs(strings) do
    output_file:write("text{ key = \"")
    output_file:write(element.key)
    output_file:write("\", value = \"")
    output_file:write(element.value)
    output_file:write("\" }\n")
  end
  output_file:write("\n");

  output_file:close()
end

return converter


-- This module reads a font list file with the format of Solarus 1.3
-- (text/fonts.dat), stores its content and deletes it since
-- it is obsolete in Solarus 1.4.
-- It also renames the 'text' directory to 'fonts'.

local converter = {}

local function file_name_to_id(file_name)
  return file_name:gsub("\.[a-zA-Z0-9]*$", ""):gsub("^.*[/\\]", "")
end

function converter.convert(quest_path)

  local font_file_names = {}  -- File names with extension.
  local font_ids = {}         -- File names without extension.

  -- Read the 1.3 file.
  function font(properties)

    local file_name = properties.file or error("Missing file field")
    local font_id = file_name_to_id(file_name)

    -- Two identical file names are allowed (only the size differed).
    if font_file_names[file_name] ~= nil then
      -- Do nothing in this case. The font is already added.
      return
    end

    font_file_names[file_name] = true

    -- But we cannot support the same base file name with different
    -- extensions.
    if font_ids[font_id] ~= nil then
      error("Duplicate font '" .. font_id .. "'")
    end

    font_ids[font_id] = true
  end

  local input_file = quest_path .. "/data/text/fonts.dat"
  dofile(input_file)

  -- Delete the file.
  local success, error_message = os.remove(input_file)
  if not success then
    error("Could not delete the obsolete font list file: " .. error_message)
  end

  -- Rename the 'text' directory into 'fonts'.
  local success, error_message = os.rename(quest_path .. "/data/text", quest_path .. "/data/fonts")
  if not success then
    error("Could not rename the 'text' directory to 'fonts': " .. error_message)
  end

  -- Move each font file into the 'fonts' directory.
  for old_file_name, _ in pairs(font_file_names) do
    old_file_name = old_file_name:gsub("^text/", "fonts/")
    local new_file_name = "fonts/" .. file_name_to_id(old_file_name)
    if new_file_name ~= old_file_name then
      local success, error_message = os.rename(quest_path .. "/data/" .. old_file_name, quest_path .. "/data/" .. new_file_name)
      if not success then
        error("Could not rename the 'text' directory to 'fonts': " .. error_message)
      end
    end

  end

  -- Return the font list so that other converters can use it.
  local font_ids_array = {}
  for font_id, _ in pairs(font_ids) do
    font_ids_array[#font_ids_array + 1] = font_id
  end
  table.sort(font_ids_array)
  return font_ids_array
end

return converter


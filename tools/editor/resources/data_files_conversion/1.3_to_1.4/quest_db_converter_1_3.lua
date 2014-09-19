-- This module reads a quest resource list file with the format of Solarus 1.3
-- (project_db.dat) and adds fonts to it, because fonts become a resource.

local converter = {}

function converter.convert(quest_path, fonts)

  -- Add fonts to the list.
  local output_file, error_message = io.open(quest_path .. "/data/project_db.dat", "a")
  if output_file == nil then
    error("Cannot open quest resource list file for writing: " .. error_message)
  end

  for _, id in ipairs(fonts) do
    output_file:write('font{ id = "' .. id .. '", description = "' .. id .. '" }\n')
  end

  output_file:close()
end

return converter


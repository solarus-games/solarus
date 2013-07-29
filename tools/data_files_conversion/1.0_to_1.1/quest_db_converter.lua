local converter = {}

local resource_type_names = {
  [0] = "map",
  [1] = "tileset",
  [2] = "music",
  [3] = "sprite",
  [4] = "sound",
  [5] = "item",
  [6] = "enemy",
  [7] = "language",
}

local function load_quest_db(quest_path)

  local resources = {}

  -- Create a table for each type of resource.
  for _, resource_type_name in pairs(resource_type_names) do
    resources[resource_type_name] = {}
  end

  local file, error_message = io.open(quest_path .. "/data/project_db.dat")
  if file == nil then
    error("Cannot read project_db.dat file: " .. error_message)
  end

  local line_number = 0
  for line in file:lines() do
    line_number = line_number + 1
    local resource_type_index, resource_id, resource_human_name =
        line:match("([0-9])\t(.*)\t(.*)")
    if resource_type_index == nil then
      error("Line " .. line_number .. ": Wrong resource type")
    end
    local resource_type_name = resource_type_names[tonumber(resource_type_index)]
    if resource_type_name == nil then
      error("Line " .. line_number .. ": Wrong resource type")
    end

    if resource_id == nil then
      error("Line " .. line_number .. ": Wrong resource id")
    end

    if resource_human_name == nil then
      error("Line " .. line_number .. ": Wrong resource name")
    end

    local resources_of_current_type = resources[resource_type_name]
    resources_of_current_type[#resources_of_current_type + 1] = {
      id = resource_id,
      human_name = resource_human_name
    }
  end

  return resources
end

function converter.convert(quest_path)

  -- Read project_db.dat.
  local resources = load_quest_db(quest_path)

  -- Write the 1.1 file.
  local output_file = io.open(quest_path .. "/data/project_db.dat", "w")

  output_file:write([[
-- This is a resource list file for Solarus 1.1.
-- This file was converted from Solarus 1.0 using the script
-- tools/data_files_conversion/1.0_to_1.1/quest_db_converter.lua.
]])

  for i = 0, 7 do
    local resource_type_name = resource_type_names[i]
    local resource = resources[resource_type_name]

    for _, element in ipairs(resource) do
      output_file:write(resource_type_name)
      output_file:write("{ id = \"")
      output_file:write(element.id)
      output_file:write("\", description = \"")
      output_file:write(element.human_name)
      output_file:write("\" }\n")
    end
    output_file:write("\n");
  end
  output_file:close()

  return resources
end

return converter


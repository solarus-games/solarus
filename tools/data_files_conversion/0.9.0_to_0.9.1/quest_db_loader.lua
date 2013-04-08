
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

-- Reads the project_db.dat file and returns a table of all quest resources.
function quest_db_loader.load_quest_db(quest_path)

  local resources = {}

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

    resources[#resources + 1] = {
      ["resource_type"] = resource_type_name,
      ["resource_id"] = resource_id,
      ["human_name"] = resource_human_name,
    }
  end

  return resources
end

return quest_db_loader


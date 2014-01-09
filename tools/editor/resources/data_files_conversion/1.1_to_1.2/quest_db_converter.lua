-- This module reads a quest resource list file with the format of solarus 1.1
-- (project_db.dat) and returns a table with its content.

local converter = {}

local resource_type_names = {
  "map",
  "tileset",
  "music",
  "sprite",
  "sound",
  "item",
  "enemy",
  "language",
}

local function load_quest_db(quest_path)

  local resources = {}

  -- Create a table for each type of resource.
  for _, resource_type_name in ipairs(resource_type_names) do
    local resource = {}
    resources[resource_type_name] = resource
    _G[resource_type_name] = function(resource_element)
      resource[#resource + 1] = resource_element
    end
  end

  dofile(quest_path .. "/data/project_db.dat")

  return resources
end

function converter.convert(quest_path, languages)

  -- Read project_db.dat.
  local resources = load_quest_db(quest_path)

  -- Add the new sound "running_obstacle.dat" unless it already exists.
  local running_obstacle_sound_exists = false
  for _, element in ipairs(resources.sound) do
    if element.id == "running_obstacle" then
      running_obstacle_sound_exists = true
    end
  end

  if not running_obstacle_sound_exists then
    local output_file, error_message = io.open(quest_path .. "/data/project_db.dat", "a")
    if output_file == nil then
      error("Cannot open quest resource list file for writing: " .. error_message)
    end
    output_file:write('sound{ id = "running_obstacle", description = "running_obstacle" }\n')
    output_file:close()
  end

  return resources
end

return converter


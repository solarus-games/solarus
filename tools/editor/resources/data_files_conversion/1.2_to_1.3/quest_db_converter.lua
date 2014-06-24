-- This module reads a quest resource list file with the format of solarus 1.2
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
  "entity",
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

-- Returns whether the specified element is declared in project_db.dat.
local function is_resource_declared(resources, resource_type, element_id)

  for _, element in ipairs(resources[resource_type]) do
    if element.id == element_id then
      return true
    end
  end

  return false
end

-- Add an element to project_db.dat if it is not already there.
local function ensure_resource_declared(output_file, resources, resource_type, element_id, element_description)

  if not is_resource_declared(resources, resource_type, element_id) then
    output_file:write(resource_type .. '{ id = "' .. element_id .. '", description = "' .. element_description .. '" }\n')
  end
end

function converter.convert(quest_path, languages)

  -- Read project_db.dat.
  local resources = load_quest_db(quest_path)

  -- Make sure that old built-in sprites and sounds of switches are declared.
    local output_file, error_message = io.open(quest_path .. "/data/project_db.dat", "a")
    if output_file == nil then
      error("Cannot open quest resource list file for writing: " .. error_message)
    end
    ensure_resource_declared(output_file, resources, "sprite", "entities/switch", "Switch")
    ensure_resource_declared(output_file, resources, "sprite", "entities/solid_switch", "Solid switch")
    ensure_resource_declared(output_file, resources, "sound", "switch", "switch")
    output_file:close()

  return resources
end

return converter


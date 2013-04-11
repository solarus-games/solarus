-- This module reads a map data file with the format of solarus 0.9.2
-- and converts it into the format of solarus 1.0.0.
-- The old format (solarus 0.9.2) is a text file with a specific syntax.
-- The new format (solarus 1.0.0) is a Lua data file.

local map_converter = {}

local line_number
local metadata  -- Map meta information.

-- Converts an old index of boolean savegame variables
-- to a valid savegame variable name.
local function prepend_b(value)
  return "b" .. value
end

-- Teletransporters no longer have a subtype.
-- They have sprite and sound instead.
local function convert_teletransporter_subtype(subtype, entity)

  if subtype == 0 then
    -- Invisible teletransporter: no sprite and no sound.
    return nil

  elseif subtype == 1 then
    -- Yellow teletransporter.
    return {
      sprite = "entities/teletransporter",
      sound = "warp",
    }

  else
    error("Line " .. line_number .. ": Illegal teletransporter subtype: '" .. subtype .. "'")
  end
end

-- Sensors no longer have a subtype.
-- They must be scripted now.
local function convert_sensor_subtype(subtype, entity)

  if subtype == 0 then
    -- Custom sensor: no problem.
    return nil
  end

  -- Sensor with a built-in behavior: tell the user that this behavior
  -- no longer exists.
  local name = ""
  for _, v in ipairs(entity) do
    if v.key == "name" then
      name = v.value
    end
  end

  io.stderr:write("Warning: Map " .. metadata.map_id .. ", line " .. line_number .. " (entity '" .. name
      .. ")': sensors no longer have built-in subtypes. You will have to script its behavior.\n") 
  return nil
end

-- Doors have completely changed. There are no more predefined door subtypes:
-- instead, everything is customizable.
local function convert_door_subtype(subtype, entity)

  if subtype ~= 2 and subtype ~= 8 then

    -- First determine x, y and direction because coordinates need to be fixed.
    local index_x, index_y, direction
    for i, v in ipairs(entity) do
      if v.key == "x" then
        index_x = i
      elseif v.key == "y" then
        index_y = i
      elseif v.key == "direction" then
        direction = v.value
      end
    end
    if index_x == nil or index_y == nil or direction == nil then
      error("Line " .. line_number .. ": Could not determine x, y or direction for this door")
    end

    if direction % 2 == 0 then
      -- Door to the west or to the east.
      entity[index_y].value = entity[index_y].value + 8
    else
      -- Door to the north of to the south.
      entity[index_x].value = entity[index_x].value + 8
    end

  end

  if subtype == 0 then
    -- Normal closed door.
    return {
      sprite = "entities/door_normal",
    }

  elseif subtype == 1 then
    -- Small key.
    return {
      sprite = "entities/door_small_key",
      opening_method = "interaction_if_savegame_variable",
      opening_condition = metadata.small_keys_variable,
      opening_condition_consumed = true,
      cannot_open_dialog_id = "_small_key_required",
    }

  elseif subtype == 2 then
    -- Small key block.
    return {
      sprite = "entities/door_small_key_block",
      opening_method = "interaction_if_savegame_variable",
      opening_condition = metadata.small_keys_variable,
      opening_condition_consumed = true,
      cannot_open_dialog_id = "_small_key_required",
    }

  elseif subtype == 3 then
    -- Big key.
    if metadata.dungeon == nil then
      error("Line " .. line_number .. ": Big key doors are only allowed in dungeons")
    end
    return {
      sprite = "entities/door_big_key",
      opening_method = "interaction_if_savegame_variable",
      opening_condition = "dungeon_" .. metadata.dungeon .. "_big_key",
      cannot_open_dialog_id = "_big_key_required",
    }

  elseif subtype == 4 then
    -- Boss key.
    if metadata.dungeon == nil then
      error("Line " .. line_number .. ": Boss key doors are only allowed in dungeons")
    end
    return {
      sprite = "entities/door_boss_key",
      opening_method = "interaction_if_savegame_variable",
      opening_condition = "dungeon_" .. metadata.dungeon .. "_boss_key",
      cannot_open_dialog_id = "_boss_key_required",
    }

  elseif subtype == 5 then
    -- Weak wall.
    return {
      sprite = "entities/door_weak_wall",
      opening_method = "explosion",
    }

  elseif subtype == 6 then
    -- Very weak wall.
    return {
      sprite = "entities/door_very_weak_wall",
      opening_method = "explosion",
    }

  elseif subtype == 8 then
    -- Weak block.
    return {
      sprite = "entities/door_weak_block",
      opening_method = "explosion",
    }

  else
    error("Line " .. line_number .. ": Illegal door subtype: '" .. subtype .. "'")
  end
end

-- The syntax of treasure names has slightly changed:
-- "_none" and "_random" no longer exist.
local function convert_treasure_name(treasure_name)

  if treasure_name == "_random" then
    -- Random is no longer built-in.
    treasure_name = "random"
  elseif treasure_name == "_none" then
    -- "_none" is just replaced by nil.
    treasure_name = nil
  end
  return treasure_name
end

-- Returns the small keys savegame variable of this map in the new savegame format.
local function convert_small_keys_variable(world, small_keys_variable)

  if world > 0 then
    -- We are in a dungeon.
    small_keys_variable = "dungeon_" .. world .. "_small_keys"
  else
    small_keys_variable = "i" .. small_keys_variable
  end
  return small_keys_variable
end

-- This table fully describes the old syntax of each entity and how to convert
-- it to the new format.
local entity_syntaxes = {

  -- Entity type id -> entity line syntax.
  [0] = {
    entity_type_name = "tile",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "pattern" },
  },

  [1] = {
    entity_type_name = "destination",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "sprite", converter = {
        [0] = nil,
        [1] = "entities/destination",
      }
    },
  },

  [2] = {
    entity_type_name = "teletransporter",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "subtype", converter = convert_teletransporter_subtype },
    { token_name = "transition" },
    { token_name = "destination_map", token_type = "string" },
    { token_name = "destination", token_type = "string" },
  },

  [3] = {
    entity_type_name = "pickable",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "treasure_name", token_type = "string", converter = convert_treasure_name },
    { token_name = "treasure_variant", nil_value = 0 },
    { token_name = "treasure_savegame_variable", token_type = "string", converter = prepend_b, nil_value = "-1" },
  },

  [4] = {
    entity_type_name = "destructible",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "subtype", converter = {
        [0] = "pot",
        [2] = "bush",
        [3] = "white_stone",
        [4] = "black_stone",
        [5] = "grass",
        [6] = "bomb_flower",
      }
    },
    { token_name = "treasure_name", token_type = "string", converter = convert_treasure_name },
    { token_name = "treasure_variant", nil_value = 0 },
    { token_name = "treasure_savegame_variable", token_type = "string", converter = prepend_b, nil_value = "-1" },
  },

  [5] = {
    entity_type_name = "chest",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "is_big_chest", token_type = "boolean" },
    { token_name = "treasure_name", token_type = "string", converter = convert_treasure_name },
    { token_name = "treasure_variant", nil_value = 0 },
    { token_name = "treasure_savegame_variable", token_type = "string", converter = prepend_b, nil_value = "-1" },
  },

  [6] = {
    entity_type_name = "jumper",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "jump_length" },
  },

  [7] = {
    entity_type_name = "enemy",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "breed", token_type = "string" },
    { token_name = "rank" },
    { token_name = "savegame_variable", token_type = "string", converter = prepend_b, nil_value = "-1" },
    { token_name = "treasure_name", token_type = "string", converter = convert_treasure_name },
    { token_name = "treasure_variant", nil_value = 0 },
    { token_name = "treasure_savegame_variable", token_type = "string", converter = prepend_b, nil_value = "-1" },
  },

  [8] = {
    entity_type_name = "npc",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "subtype", token_type = "string" },
    { token_name = "sprite", token_type = "string", nil_value = "_none" },
    { token_name = "behavior", token_type = "string" },
  },

  [9] = {
    entity_type_name = "block",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "sprite", token_type = "string", nil_value = "_none" },
    { token_name = "pushable", token_type = "boolean" },
    { token_name = "pullable", token_type = "boolean" },
    { token_name = "maximum_moves" },
  },

  [10] = {
    entity_type_name = "dynamic_tile",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "pattern" },
    { token_name = "enabled_at_start", token_type = "boolean" },
  },

  [11] = {
    entity_type_name = "switch",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "subtype", token_type = "string" },
    { token_name = "needs_block", token_type = "boolean" },
    { token_name = "inactivate_when_leaving", token_type = "boolean" },
  },

  [12] = {
    entity_type_name = "wall",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "stops_hero", token_type = "boolean" },
    { token_name = "stops_enemies", token_type = "boolean" },
    { token_name = "stops_npcs", token_type = "boolean" },
    { token_name = "stops_blocks", token_type = "boolean" },
  },

  [13] = {
    entity_type_name = "sensor",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "name", token_type = "string" },
    { token_name = "subtype", converter = convert_sensor_subtype },
  },

  [14] = {
    entity_type_name = "crystal",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
  },

  [15] = {
    entity_type_name = "crystal_block",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "width" },
    { token_name = "height" },
    { token_name = "subtype", token_type = "string" },
  },

  [16] = {
    entity_type_name = "shop_item",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "treasure_name", token_type = "string", converter = convert_treasure_name },
    { token_name = "treasure_variant", nil_value = 0 },
    { token_name = "treasure_savegame_variable", token_type = "string", converter = prepend_b, nil_value = "-1" },
    { token_name = "price" },
    { token_name = "dialog", token_type = "string" },
  },

  [17] = {
    entity_type_name = "conveyor_belt",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "direction" },
  },

  [18] = {
    entity_type_name = "door",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "subtype", converter = convert_door_subtype },
    { token_name = "savegame_variable", token_type = "string", converter = prepend_b, nil_value = "-1" },
  },

  [19] = {
    entity_type_name = "stairs",
    { token_name = "layer" },
    { token_name = "x" },
    { token_name = "y" },
    { token_name = "name", token_type = "string" },
    { token_name = "direction" },
    { token_name = "subtype", token_type = "string" },
  },

}

-- Old world number -> new world name.
local world_names = {
  [-1] = "inside_world",
  [0] = "outside_world"
}
for i = 1, 20 do
  world_names[i] = "dungeon_" .. i
end

-- Old floor number -> new floor number.
-- -100 and -99 are replaced by nil.
local floors = {
}
for i = -16, 15 do
  floors[i] = i
end

-- Parses the metadata line from the old syntax.
-- Returns a table containing the map metadata.
local function parse_metadata(line)

  local metadata = {}

  local width, height, world, floor, x, y, small_keys_variable, tileset, music =
    line:match("^([0-9]+)%s+([0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([-0-9]+)%s+([a-zA-Z0-9_]+)%s+([a-zA-Z0-9_.]+)")

  if width == nil then
    error("Line " .. line_number .. ": Invalid map metadata")
  end

  -- Store all the meta information with the new semantics.
  metadata.x = tonumber(x)
  metadata.y = tonumber(y)
  metadata.width = tonumber(width)
  metadata.height = tonumber(height)
  local world_number = tonumber(world)
  metadata.world = world_names[world_number]
  if world_number > 0 then
    metadata.dungeon = world_number
  end
  metadata.floor = floors[tonumber(floor)]

  if small_keys_variable ~= -1 then
    metadata.small_keys_variable = convert_small_keys_variable(world_number, tonumber(small_keys_variable))
  end

  metadata.tileset = tileset

  if music == "none" then
    music = nil
  else
    -- Remove the extension of the file name.
    music = music:gsub("%.[^.]*$", "")
  end
  metadata.music = music

  return metadata
end

-- Outputs the metadata part with the new syntax.
local function print_metadata(output_file)

  output_file:write("properties{\n")
  output_file:write("  x = " .. metadata.x .. ",\n")
  output_file:write("  y = " .. metadata.y .. ",\n")
  output_file:write("  width = " .. metadata.width .. ",\n")
  output_file:write("  height = " .. metadata.height .. ",\n")
  output_file:write("  world = \"" .. metadata.world .. "\",\n")
  if metadata.floor ~= nil then
    output_file:write("  floor = " .. metadata.floor .. ",\n")
  end
  output_file:write("  tileset = \"" .. metadata.tileset .. "\",\n")
  if metadata.music ~= nil then
    output_file:write("  music = \"" .. metadata.music .. "\",\n")
  end
  output_file:write("}\n\n")
end

-- Parses a map entity from a line with the old syntax.
local function parse_entity(line)

  local entity = {}
  local entity_type_id = nil
  local i = 1
  local syntax
  for token in line:gmatch("([^%s]+)%s*") do
    if entity_type_id == nil then
      -- First token: entity type.
      entity_type_id = tonumber(token)
      if entity_type_id == nil then
        error("Line " .. line_number .. ": Invalid entity type id")
      end
      syntax = entity_syntaxes[entity_type_id]
      entity.entity_type_name = syntax.entity_type_name
    else
      local token_type = syntax[i].token_type 
      local value
      if token_type == "string" then
        value = token
      elseif token_type == "boolean" then
        if token ~= "0" then
          value = true
        else
          value = false
        end
      elseif token_type == "integer" or token_type == nil then  -- Default is integer.
        value = tonumber(token)
        if value == nil then
          error("Line " .. line_number .. ": Number expected for token '" ..
              syntax[i].token_name .. "'")
        end
      else
        error("Line " .. line_number .. ": Unknown type '" .. token_type ..
            " for token '" .. syntax[i].token_name .. "'")
      end

      if value == syntax[i].nil_value then
        entity[#entity + 1] = {
          key = syntax[i].token_name,
          value = nil
        }
      else
        if syntax[i].converter ~= nil then
          -- The value has to be converted to a new syntax.
          if type(syntax[i].converter) == "table" then
            value = syntax[i].converter[value]
          else
            value = syntax[i].converter(value, entity)
          end
        end

        if type(value) == "table" then
          -- Multi-valued element.
          for k, v in pairs(value) do
            entity[#entity + 1] = {
              key = k,
              value = v
            }
          end
        else
          -- Single value.
          entity[#entity + 1] = {
            key = syntax[i].token_name,
            value = value
          }
        end
      end
      i = i + 1
    end
  end

  return entity
end

-- Outputs a map entity with the new syntax.
local function print_entity(output_file, entity)

  output_file:write("" .. entity.entity_type_name .. "{\n")
  for i, v in ipairs(entity) do
    local value
    if type(v.value) == "string" then
      value = "\"" .. v.value .. "\""
    elseif type(v.value) == "boolean" then
      if v.value then
	value = "true"
      else
	value = "false"
      end
    else  -- Integer or nil.
      value = v.value
    end
    if value ~= nil then
      output_file:write("  " .. v.key .. " = " .. value .. ",\n")
    end
  end
  output_file:write("}\n\n")
end

function map_converter.convert(quest_path, map_id)

  line_number = 1
  metadata = nil

  local input_file_name = string.format(quest_path .. "/data/maps/map%04d.dat", map_id)
  local input_file, error_message = io.open(input_file_name)
  if input_file == nil then
    error("Cannot open old map file for reading: " .. error_message)
  end
  local lines = input_file:lines()

  local output_file_name = quest_path .. "/data/maps/" .. map_id .. ".dat"
  local output_file, error_message = io.open(output_file_name, "w")
  if output_file == nil then
    error("Cannot open new map file for writing: " .. error_message)
  end

  -- The first line is the map metadata.
  local first_line = lines()
  if first_line == nil then
    error("Empty map data file")
  end
  metadata = parse_metadata(first_line)
  metadata.map_id = map_id

  print_metadata(output_file)

  for line in lines do
    line_number = line_number + 1
    local entity = parse_entity(line)
    print_entity(output_file, entity)
  end
end

return map_converter


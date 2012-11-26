local game = ...

function game:on_started()

  -- Set up the HUD.
  local hearts_class = require("hud/hearts")
  local magic_bar_class = require("hud/magic_bar")
  local rupees_class = require("hud/rupees")
  local floor_class = require("hud/floor")

  self.hud = {}

  self.hud.hearts = hearts_class:new(self)
  self.hud.hearts:set_dst_position(-104, 6)

  self.hud.magic_bar = magic_bar_class:new(self)
  self.hud.magic_bar:set_dst_position(-104, 27)

  self.hud.rupees = rupees_class:new(self)
  self.hud.rupees:set_dst_position(8, -20)

  self.hud.floor = floor_class:new(self)
  self.hud.floor:set_dst_position(5, 70)

  self:set_hud_enabled(true)
end

-- This event is called when a new map has just become active.
function game:on_map_changed(map)

  -- Notify the hud.
  if self.hud_enabled then
    for _, menu in pairs(self.hud) do
      if menu.on_map_changed ~= nil then
        menu:on_map_changed(map)
      end
    end
  end
end

-- Useful functions for this specific quest.

function game:get_player_name()
  return self:get_value("player_name")
end

function game:set_player_name(player_name)
  self:set_value("player_name", player_name)
end

function game:is_dungeon_finished(dungeon)
  return self:get_value("dungeon_" .. dungeon .. "_finished")
end

function game:set_dungeon_finished(dungeon, finished)
  if finished == nil then
    finished = true
  end
  self:set_value("dungeon_" .. dungeon .. "_finished", finished)
end

-- Returns whether the current map is in a dungeon.
function game:is_in_dungeon()
  return self:get_dungeon() ~= nil
end

-- Returns whether the current map is in the inside world.
function game:is_in_inside_world()
  return self:get_map():get_world() == "inside_world"
end

-- Returns whether the current map is in the outside world.
function game:is_in_outside_world()
  return self:get_map():get_world() == "outside_world"
end

-- Returns the index of the current dungeon if any, or nil.
function game:get_dungeon()

  local world = self:get_map():get_world()
  local index = world:match("^dungeon_([0-9]+)$")
  if index == nil then
    return nil
  end

  return tonumber(index)
end

-- Returns whether a small key counter exists on the current map.
function game:are_small_keys_enabled()
  return self:get_small_keys_savegame_variable() ~= nil
end

-- Returns the name of the integer variable that stores the number
-- of small keys for the current map, or nil.
function game:get_small_keys_savegame_variable()

  local map = self:get_map()

  -- Does the map explicitely defines a small key counter?
  if map.small_keys_savegame_variable ~= nil then
    return map.small_keys_savegame_variable
  end

  -- Are we in a dungeon?
  local dungeon = self:get_dungeon()
  if dungeon ~= nil then
    return "dungeon_" .. dungeon .. "_small_keys"
  end

  -- No small keys on this map.
  return nil
end

-- Returns whether the player has at least one small key.
-- Raises an error is small keys are not enabled in the current map.
function game:has_small_key()

  if not self:are_small_keys_enabled() then
    error("Small keys are not enabled in the current map")
  end

  local savegame_variable = self:get_small_keys_savegame_variable()
  return self:get_value(savegame_variable) > 0
end

-- Adds a small key to the player.
-- Raises an error is small keys are not enabled in the current map.
function game:add_small_key()

  if not self:are_small_keys_enabled() then
    error("Small keys are not enabled in the current map")
  end

  local savegame_variable = self:get_small_keys_savegame_variable()
  local num_small_keys = self:get_value(savegame_variable) + 1
  self:set_value(savegame_variable, num_small_keys)
end

-- Removes a small key to the player.
-- Raises an error is small keys are not enabled in the current map
-- or if the player has no small keys.
function game:remove_small_key()

  if not self:has_small_key() then
    error("The player has no small key")
  end

  local savegame_variable = self:get_small_keys_savegame_variable()
  local num_small_keys = self:get_value(savegame_variable) - 1
  self:set_value(savegame_variable, num_small_keys)
end

function game:is_hud_enabled()
  return self.hud_enabled
end

function game:set_hud_enabled(hud_enabled)

  if hud_enabled ~= game.hud_enabled then
    game.hud_enabled = hud_enabled

    for _, menu in pairs(self.hud) do
      if hud_enabled then
	sol.menu.start(self, menu)
      else
	sol.menu.stop(menu)
      end
    end
  end
end

-- Returns a bottle with the specified content, or nil.
function game:get_first_bottle_with(variant)

  for i = 1, 4 do
    local item = self:get_item("bottle_" .. i)
    if item:get_variant() == variant then
      return item
    end
  end

  return nil
end

function game:get_first_empty_bottle()
  return self:get_first_bottle_with(1)
end

function game:has_bottle()

  for i = 1, 4 do
    local item = self:get_item("bottle_" .. i)
    if item:has_variant() then
      return item
    end
  end

  return nil
end

function game:has_bottle_with(variant)

  return self:get_first_bottle_with(variant) ~= nil
end

-- Run the game.
sol.main.game = game
game:start()


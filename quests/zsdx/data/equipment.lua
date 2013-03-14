local game = ...

-- Returns whether a small key counter exists on the current map.
function game:are_small_keys_enabled()
  return self:get_small_keys_savegame_variable() ~= nil
end

-- Returns the name of the integer variable that stores the number
-- of small keys for the current map, or nil.
function game:get_small_keys_savegame_variable()

  local map = self:get_map()

  if map ~= nil then
    -- Does the map explicitely defines a small key counter?
    if map.small_keys_savegame_variable ~= nil then
      return map.small_keys_savegame_variable
    end

    -- Are we in a dungeon?
    local dungeon_index = self:get_dungeon_index()
    if dungeon_index ~= nil then
      return "dungeon_" .. dungeon_index .. "_small_keys"
    end
  end

  -- No small keys on this map.
  return nil
end

-- Returns whether the player has at least one small key.
-- Raises an error is small keys are not enabled in the current map.
function game:has_small_key()

  return self:get_num_small_keys() > 0
end

-- Returns the number of small keys of the player.
-- Raises an error is small keys are not enabled in the current map.
function game:get_num_small_keys()

  if not self:are_small_keys_enabled() then
    error("Small keys are not enabled in the current map")
  end

  local savegame_variable = self:get_small_keys_savegame_variable()
  return self:get_value(savegame_variable) or 0
end

-- Adds a small key to the player.
-- Raises an error is small keys are not enabled in the current map.
function game:add_small_key()

  if not self:are_small_keys_enabled() then
    error("Small keys are not enabled in the current map")
  end

  local savegame_variable = self:get_small_keys_savegame_variable()
  self:set_value(savegame_variable, self:get_num_small_keys() + 1)
end

-- Removes a small key to the player.
-- Raises an error is small keys are not enabled in the current map
-- or if the player has no small keys.
function game:remove_small_key()

  if not self:has_small_key() then
    error("The player has no small key")
  end

  local savegame_variable = self:get_small_keys_savegame_variable()
  self:set_value(savegame_variable, self:get_num_small_keys() - 1)
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
    if self:has_item("bottle_" .. i) then
      return true
    end
  end

  return false
end

function game:has_bottle_with(variant)

  return self:get_first_bottle_with(variant) ~= nil
end


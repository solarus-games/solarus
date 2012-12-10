-- This script handles all bottles (each bottle script runs it).

local item = ...
item.bottle_to_make_empty = nil  -- Name of a bottle to make empty at the next cycle.

function item:on_using()

  local variant = self:get_variant()
  local game = self:get_game()
  local map = self:get_map()

  -- empty bottle
  if variant == 1 then
    sol.audio.play_sound("wrong")
    self:set_finished()

    -- water
  elseif variant == 2 then
    -- ask the hero to pour away the water
    map:start_dialog("use_bottle_with_water", function(answer)
      if answer == 0 then
	-- empty the water
	self:set_variant(1) -- make the bottle empty
	sol.audio.play_sound("item_in_water")
      end
      self:set_finished()
    end)

    -- red potion
  elseif variant == 3 then
    game:add_life(game:get_max_life())
    self:set_variant(1)
    self:set_finished()

    -- green potion
  elseif variant == 4 then
    game:add_magic(game:get_max_magic())
    self:set_variant(1)
    self:set_finished()

    -- blue potion
  elseif variant == 5 then
    game:add_life(game:get_max_life())
    self:set_variant(1)
    game:add_magic(game:get_max_magic())
    self:set_finished()

    -- fairy
  elseif variant == 6 then

    -- release the fairy
    local x, y, layer = map:get_entity("hero"):get_position()
    map:create_pickable{
      treasure_name = "fairy",
      treasure_variant = 1,
      x = x,
      y = y,
      layer = layer
    }
    self:set_variant(1) -- make the bottle empty
    self:set_finished()
  end
end

function item:on_npc_interaction(npc)

  if npc:get_name():find("^water_for_bottle") then
    local game = self:get_game()
    local map = self:get_map()
    -- The hero interacts with a place where he can get some water.
    if game:has_bottle() then
      local first_empty_bottle = game:get_first_empty_bottle()
      if first_empty_bottle ~= nil then
        map:start_dialog("found_water", function(answer)
	  if answer == 0 then
	    map:treasure_give(first_empty_bottle:get_name(), 2, -1)
	  end
	end)
      else
        map:start_dialog("found_water.no_empty_bottle")
      end
    else
      map:start_dialog("found_water.no_bottle")
    end
  end
end

function item:on_npc_interaction_item(npc, item_used)

  if item_used:get_name():find("^bottle")
      and name:get_name():find("^water_for_bottle") then
    -- the hero interacts with a place where he can get some water:
    -- no matter whether he pressed the action key or the item key of a bottle, we do the same thing
    self:on_npc_interaction(npc)
    return true
  end

  return false
end

function item:on_variant_changed(variant)

  -- The possession state of a bottle has changed:
  -- see if the player has at least a fairy.
  -- TODO remove this when the gameover screen is scripted.
  if self:get_game():has_bottle_with(6) then
    self:get_game():set_ability("get_back_from_death", 1)
  else
    self:get_game():set_ability("get_back_from_death", 0)
  end
end

function item:on_ability_used(ability_name)

  -- TODO remove this when the gameover screen is scripted.
  -- If the hero was just saved by a fairy,
  -- let's find a bottle with a fairy and make it empty.

  -- Remember that all bottles are notified, but only
  -- one should remove its fairy.

  if ability_name == "get_back_from_death"
    and self:get_game():has_ability("get_back_from_death") then

    self.bottle_to_make_empty = self:get_game():get_first_bottle_with(6)
  end
end

function item:on_update()

  -- TODO remove this when the gameover screen is scripted.
  if self.bottle_to_make_empty ~= nil then
    self.bottle_to_make_empty:set_variant(1)
    self.bottle_to_make_empty = nil
  end
end


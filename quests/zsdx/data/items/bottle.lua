-- This script handles all bottles (each bottle script includes it)

local item = ...
item.bottle_to_make_empty = "" -- name of a bottle to make empty at the next cycle

function item:on_use()

  local variant = self:get_variant()
  local game = self:get_game()

  -- empty bottle
  if variant == 1 then
    sol.audio.play_sound("wrong")
    self:set_finished()

    -- water
  elseif variant == 2 then
    -- ask the hero to pour away the water
    self:get_map():dialog_start("use_bottle_with_water", self)

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
    local x, y, layer = self:get_map():hero_get_position();
    self:get_map():pickable_item_create("fairy", 1, -1, x, y, layer);
    self:set_variant(1) -- make the bottle empty
    self:set_finished()
  end
end

function item:on_dialog_finished(dialog_id, answer)

  if dialog_id == "use_bottle_with_water" then

    if answer == 0 then

      -- empty the water
      self:set_variant(1) -- make the bottle empty
      sol.audio.play_sound("item_in_water")

    end
    self:set_finished()

  elseif dialog_id == "found_water" then

    if answer == 0 then
      self:get_map():treasure_give(self:get_first_empty_bottle(), 2, -1)
    end
  
  end
end

function item:on_npc_interaction(npc_name)

  if string.find(npc_name, "^water_for_bottle") then
    -- the hero interacts with a place where he can get some water
    if self:has_bottle() then
      if self:has_empty_bottle() then
        self:get_map():dialog_start("found_water", self)
      else
        self:get_map():dialog_start("found_water.no_empty_bottle", self)
      end
    else  
      self:get_map():dialog_start("found_water.no_bottle", self)
    end
  end
end

function item:on_npc_interaction_item(npc_name, item_name, variant)

  if string.find(item_name, "^bottle") and string.find(npc_name, "^water_for_bottle") then
    -- the hero interacts with a place where he can get some water:
    -- no matter whether he pressed the action key or the item key of a bottle, we do the same thing
    self:on_npc_interaction(npc_name)
    return true
  end

  return false
end

function item:on_variant_changed(variant)

  -- the possession state of a bottle has changed: see if the player has at least a fairy
  if self:has_bottle_with(6) then
    self:get_game():set_ability("get_back_from_death", 1)
  else
    self:get_game():set_ability("get_back_from_death", 0)
  end
end

function item:on_ability_used(ability_name)

  -- if the hero was just saved by a fairy,
  -- let's find a bottle with a fairy and make it empty

  -- remember that all bottles are notified, but only
  -- one should remove its fairy

  if ability_name == "get_back_from_death"
    and self:get_game():has_ability("get_back_from_death") then

    self.bottle_to_make_empty = self:get_first_bottle_with(6)
  end
end

function item:on_update()

  if self.bottle_to_make_empty ~= "" then
    self:get_game():set_item(self.bottle_to_make_empty, 1)
    self.bottle_to_make_empty = ""
  end
end

function item:has_bottle()

  return self:get_game():has_item("bottle_1")
    or self:get_game():has_item("bottle_2")
    or self:get_game():has_item("bottle_3")
    or self:get_game():has_item("bottle_4")
end

function item:has_empty_bottle()

  return self:get_first_empty_bottle() ~= ""
end

function item:has_bottle_with(variant)

  return self:get_first_bottle_with(variant) ~= ""
end

function item:get_first_empty_bottle()

  return self:get_first_bottle_with(1)
end

function item:get_first_bottle_with(variant)

  local result = ""

  if self:get_game():get_item("bottle_1") == variant then
    result = "bottle_1"
  elseif self:get_game():get_item("bottle_2") == variant then
    result = "bottle_2"
  elseif self:get_game():get_item("bottle_3") == variant then
    result = "bottle_3"
  elseif self:get_game():get_item("bottle_4") == variant then
    result = "bottle_4"
  end

  return result
end


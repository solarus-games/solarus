local item = ...
-- Script of the Lamp

item.temporary_lit_torches = {} -- List of torches that will be unlit by timers soon (FIFO).
item.was_dark_room = false

function item:on_created()

  self:set_savegame_variable("i1104")
  self:set_assignable(true)
end

-- Called when the hero uses the Lamp.
function item:on_using()

  local magic_needed = 2  -- Number of magic points required
  if self:get_game():get_magic() >= magic_needed then
    sol.audio.play_sound("lamp")
    self:get_game():remove_magic(magic_needed)
    self:create_fire()
  else
    sol.audio.play_sound("wrong")
  end
  self:set_finished()
end

-- Creates some fire on the map.
function item:create_fire()

  local hero = self:get_map():get_entity("hero")
  local direction = hero:get_direction()
  local dx, dy
  if direction == 0 then
    dx, dy = 18, -4
  elseif direction == 1 then
    dx, dy = 0, -24
  elseif direction == 2 then
    dx, dy = -20, -4
  else
    dx, dy = 0, 16
  end

  local x, y, layer = hero:get_position()
  self:get_map():create_fire{
    x = x + dx,
    y = y + dy,
    layer = layer
  }
end

-- Unlights the oldest torch still lit.
function item:unlight_oldest_torch()

  -- Remove the torch from the FIFO.
  local npc = table.remove(self.temporary_lit_torches, 1)
  if npc:exists() then
    -- Change its animation if it still exists on the map.
    npc:get_sprite():set_animation("unlit")
  end

  if #self.temporary_lit_torches == 0 and self.was_dark_room then
    -- make the room dark again
    self:get_map():set_light(0)
  end
end

-- Called when the player obtains the Lamp.
function item:on_obtained(variant, savegame_variable)

  -- Give the magic bar if necessary.
  local magic_bar = self:get_game():get_item("magic_bar")
  if not magic_bar:has_variant() then
    magic_bar:set_variant(1)
  end
end

-- Called when the current map changes.
function item:on_map_changed()

  self.temporary_lit_torches = {}
  self.was_dark_room = false
end

-- Called when the hero presses the action key in front of an NPC
-- linked to the Lamp.
function item:on_npc_interaction(npc)

  if npc:get_name():find("^torch") then
    npc:get_map():start_dialog("torch.need_lamp")
  end
end

-- Called when fire touches an NPC linked to the Lamp.
function item:on_npc_collision_fire(npc)

  if npc:get_name():find("^torch") then

    local torch_sprite = npc:get_sprite()
    if torch_sprite:get_animation() == "unlit" then
      -- Temporarily light the torch up.
      torch_sprite:set_animation("lit")
      sol.timer.start(10000, function()
        self:unlight_oldest_torch()
      end)
      table.insert(self.temporary_lit_torches, npc)

      if self:get_map():get_light() == 0 then
        -- Light the room.
        self.was_dark_room = true
        self:get_map():set_light(1)
      end
    end
  end
end


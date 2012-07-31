local item = ...
-- Script of the Lamp

item.temporary_lit_torches = {} -- list of torches that will be unlit by timers soon (FIFO)
item.was_dark_room = false
item.timers = {}

-- Called when the hero uses the Lamp
function item:on_use()

  local magic_needed = 2 -- number of magic points required
  if self:get_game():get_magic() >= magic_needed then
    sol.audio.play_sound("lamp")
    self:get_game():remove_magic(magic_needed)
    self:create_fire()
  else
    sol.audio.play_sound("wrong")
  end
  self:set_finished()
end

-- Creates some fire on the map
function item:create_fire()

  local direction = self:get_map():hero_get_direction()
  if direction == 0 then
    dx, dy = 18, -4
  elseif direction == 1 then
    dx, dy = 0, -24
  elseif direction == 2 then
    dx, dy = -20, -4
  else
    dx, dy = 0, 16
  end

  local x, y, layer = self:get_map():hero_get_position()
  self:get_map():fire_create(x + dx, y + dy, layer)
end

-- Unlights the oldest torch still lit
function item:unlight_oldest_torch()

  local entity = table.remove(self.temporary_lit_torches, 1)  -- remove the torch from the FIFO
  if self:get_map():npc_exists(entity) then                     -- see if it still exists
    local torch_sprite = self:get_map():npc_get_sprite(entity)  -- get its sprite
    torch_sprite:set_animation("unlit")                  -- change the animation
  end

  if #self.temporary_lit_torches == 0 and self.was_dark_room then
    -- make the room dark again
    self:get_map():light_set(0)
  end
end

-- Called when the player obtains the Lamp
function item:on_obtained(variant, savegame_variable)
  
  -- give the magic bar if necessary
  if self:get_game():get_max_magic() == 0 then
    self:get_game():set_max_magic(42)
  end
end

-- Called when the current map changes
function item:on_map_changed()

  -- cancel all torch timers so that the previous map does not interfer with
  -- the new one
  for _, t in ipairs(self.timers) do t:stop() end
  self.temporary_lit_torches = {}
  self.was_dark_room = false
  -- TODO this should now be useless (timers are associated to maps)
end

-- Called when the hero presses the action key in front of any NPC
-- that wants to notify the lamp
function item:on_npc_interaction(npc_name)

  if string.find(npc_name, "^torch") then
    self:get_map():dialog_start("torch.need_lamp")
  end
end

-- Called when fire touches an NPC linked to the Lamp
function item:on_npc_collision_fire(npc_name)

  if string.find(npc_name, "^torch") then
    
    local torch_sprite = self:get_map():npc_get_sprite(npc_name)
    if torch_sprite:get_animation() == "unlit" then
      -- temporarily light the torch up
      torch_sprite:set_animation("lit")
      self.timers[#self.timers + 1] = sol.timer.start(10000, unlight_oldest_torch)
      table.insert(self.temporary_lit_torches, npc_name)

      if self:get_map():light_get() == 0 then
        -- light the room
        self.was_dark_room = true
        self:get_map():light_set(1)
      end
    end
  end
end


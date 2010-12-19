-- Script of the Lamp

temporary_lit_torches = {} -- list of torches that will be unlight by timers soon (FIFO)

-- Tries to use the Lamp, returns true if success
function try_use_lamp()

  magic_needed = 1 -- number of magic points required
  if sol.game.get_magic() >= magic_needed then
    sol.main.play_sound("lamp")
    sol.game.remove_magic(magic_needed)
    return true
  end
  
  sol.main.play_sound("wrong")
  return false
end

-- Unlights the oldest torch still lit
function unlight_oldest_torch()
  entity = table.remove(temporary_lit_torches, 1)                   -- remove the torch from the FIFO
  if sol.map.interactive_entity_exists(entity) then                 -- see if it still exists
    torch_sprite = sol.map.interactive_entity_get_sprite(entity)    -- get its sprite
    sol.main.sprite_set_animation(torch_sprite, "unlit")            -- change the animation
  end
end

-- Called when the player obtains the Lamp
function event_obtained(variant, savegame_variable)

  -- give the ability to see in the dark
  sol.game.set_ability("see_in_dark", 1)
  
  -- give the magic bar if necessary
  if sol.game.get_max_magic() == 0 then
    sol.game.set_max_magic(42)
  end
end

-- Called when the current map changes
function event_map_changed()
  -- cancel all torch timers so that the previous map does not interfer with the new one
  sol.main.timer_stop_all()
  temporary_lit_torches = {}
end

-- Called when the hero uses the Lamp normally
function event_use()
  try_use_lamp()
  sol.item.set_finished()
end

-- Called when the hero presses the action key in front of any interactive entity
-- that wants to notify the lamp
function event_hero_interaction(entity_name)

  if string.match(entity_name, "^torch") then
    sol.map.dialog_start("torch.need_lamp")
  end
end

-- Called when the hero uses any inventory item in front of an interactive entity
-- sensible to the Lamp
function event_hero_interaction_item(entity_name, item_name, variant)

  if item_name == "lamp" and string.match(entity_name, "^torch") then
    -- using the Lamp on a torch
    success = try_use_lamp()
    if success then
      -- lamp successfully used
      torch_sprite = sol.map.interactive_entity_get_sprite(entity_name)
      if sol.main.sprite_get_animation(torch_sprite) == "unlit" then
        -- temporarily light the torch up
        sol.main.sprite_set_animation(torch_sprite, "lit")
        sol.main.timer_start(10000, "unlight_oldest_torch", false)
        table.insert(temporary_lit_torches, entity_name)
      end
    end
    return true
  end
  return false
end


------------------
-- Dungeon 3 1F --
------------------

-- Returns whether all fives torches are on
function are_all_torches_on()

  return sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_1")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_2")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_3")) == "lit"
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_4")) == "lit" 
      and sol.main.sprite_get_animation(sol.map.interactive_entity_get_sprite("torch_5")) == "lit" 
end

-- Makes all five torches on forever
function lock_torches()
  -- the trick: just remove the interactive torches because there are static ones below
  sol.map.interactive_entity_remove("torch_1")
  sol.map.interactive_entity_remove("torch_2")
  sol.map.interactive_entity_remove("torch_3")
  sol.map.interactive_entity_remove("torch_4")
  sol.map.interactive_entity_remove("torch_5")
end

-- Called when the map starts 
function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(113) then
    -- the door of the five torches is open
    lock_torches()
  end
end

-- Called when the opening transition of the map finished
function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_3")
  end
end

-- Called after the hero uses an item on an interactive entity on this map
function event_hero_interaction_item_finished(entity_name, item_name, variant)

  if item_name == "lamp"
      and string.match(entity_name, "^torch")
      and not sol.map.door_is_open("torches_door")
      and are_all_torches_on() then

    sol.main.play_sound("secret")
    sol.map.door_open("torches_door")
    lock_torches()
  end
end


------------------
-- Dungeon 3 1F --
------------------

camera_timer = nil
remove_water_delay = 500 -- delay between each step when some water is disappearing

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

  if sol.game.savegame_get_boolean(121) then
    -- the water at the center is removed
    sol.map.tile_set_group_enabled("c_water", false)
    sol.map.tile_set_group_enabled("c_water_out", true)
  end

  if sol.game.savegame_get_boolean(122) then
    -- the east water is removed
    sol.map.tile_set_group_enabled("e_water", false)
    sol.map.tile_set_group_enabled("e_water_out", true)
  end

  if sol.game.savegame_get_boolean(131) then
    -- the north water is removed
    sol.map.tile_set_group_enabled("n_water", false)
    sol.map.tile_set_group_enabled("n_water_out", true)
  end

end

-- Called when the opening transition of the map finished
function event_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    sol.map.dialog_start("dungeon_3")
  end
end

function event_update()

  if not sol.game.savegame_get_boolean(113)
    and are_all_torches_on() then

    sol.main.play_sound("secret")
    sol.map.door_open("torches_door")
    lock_torches()
  end
end

function event_switch_activated(switch_name)

  if switch_name == "se_door_switch" and not sol.map.door_is_open("se_door") then
    sol.map.camera_move(800, 728, 150)
    camera_timer = "open_se_door"
  elseif switch_name == "ce_door_switch" and not sol.map.door_is_open("ce_door") then
    sol.map.camera_move(736, 552, 250)
    camera_timer = "open_ce_door"
  elseif switch_name == "ce_door_switch_2" and not sol.map.door_is_open("ce_door") then
    sol.map.door_open("ce_door")
    sol.main.play_sound("secret")
    sol.map.switch_set_activated("ce_door_switch", true)
  elseif switch_name == "c_water_switch" and not sol.game.savegame_get_boolean(121) then
    sol.map.camera_move(344, 736, 250)
    camera_timer = "remove_c_water"
  end
end

function event_camera_reached_target()
  sol.main.timer_start(1000, camera_timer, false)
end

function open_se_door()
  sol.main.play_sound("secret")
  sol.map.door_open("se_door")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function open_ce_door()
  sol.main.play_sound("secret")
  sol.map.door_open("ce_door")
  sol.map.switch_set_activated("ce_door_switch_2", true)
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function remove_c_water()
  sol.main.play_sound("water_drain_begin")
  sol.main.play_sound("water_drain")
  sol.map.tile_set_enabled("c_water_out", true)
  sol.map.tile_set_enabled("c_water_source", false)
  sol.main.timer_start(remove_water_delay, "remove_c_water_2", false)
end

function remove_c_water_2()
  sol.map.tile_set_enabled("c_water_middle", false)
  sol.main.timer_start(remove_water_delay, "remove_c_water_3", false)
end

function remove_c_water_3()
  sol.map.tile_set_enabled("c_water", false)
  sol.map.tile_set_enabled("c_water_less_1", true)
  sol.main.timer_start(remove_water_delay, "remove_c_water_4", false)
end

function remove_c_water_4()
  sol.map.tile_set_enabled("c_water_less_1", false)
  sol.map.tile_set_enabled("c_water_less_2", true)
  sol.main.timer_start(remove_water_delay, "remove_c_water_5", false)
end

function remove_c_water_5()
  sol.map.tile_set_enabled("c_water_less_2", false)
  sol.map.tile_set_enabled("c_water_less_3", true)
  sol.main.timer_start(remove_water_delay, "remove_c_water_6", false)
end

function remove_c_water_6()
  sol.map.tile_set_enabled("c_water_less_3", false)
  sol.game.savegame_set_boolean(121, true)
  sol.main.play_sound("secret")
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end


local map = ...
-- Dungeon 3 1F

remove_water_delay = 500 -- delay between each step when some water is disappearing

-- Returns whether all five torches are on
function are_all_torches_on()

  return map:npc_exists("torch_1")
      and map:npc_get_sprite("torch_1"):get_animation() == "lit"
      and map:npc_get_sprite("torch_2"):get_animation() == "lit"
      and map:npc_get_sprite("torch_3"):get_animation() == "lit"
      and map:npc_get_sprite("torch_4"):get_animation() == "lit" 
      and map:npc_get_sprite("torch_5"):get_animation() == "lit" 
end

-- Makes all five torches on forever
function lock_torches()
  -- the trick: just remove the interactive torches because there are static ones below
  map:npc_remove("torch_1")
  map:npc_remove("torch_2")
  map:npc_remove("torch_3")
  map:npc_remove("torch_4")
  map:npc_remove("torch_5")
end

-- Called when the map starts 
function map:on_started(destination_point_name)

  if map:get_game():get_boolean(904) then
    -- the door before the five torches is open
    map:switch_set_activated("ce_door_switch", true)
  end

  if map:get_game():get_boolean(113) then
    -- the door after the five torches is open
    lock_torches()
  end

  if map:get_game():get_boolean(121) then
    -- the water at the center is removed
    map:tile_set_group_enabled("c_water", false)
    map:tile_set_group_enabled("c_water_out", true)
    map:jumper_set_group_enabled("c_water_on_jumper", false)
  else
    map:obstacle_set_group_enabled("c_water_off_obstacle", false)
  end

  if map:get_game():get_boolean(122) then
    -- the east water is removed
    map:tile_set_group_enabled("e_water", false)
    map:tile_set_group_enabled("e_water_out", true)
    map:jumper_set_group_enabled("e_water_on_jumper", false)
  else
    map:obstacle_set_group_enabled("e_water_off_obstacle", false)
  end

  if map:get_game():get_boolean(131) then
    -- the north water is removed
    map:tile_set_group_enabled("n_water", false)
    map:tile_set_group_enabled("n_water_out", true)
    map:jumper_set_group_enabled("n_water_on_jumper", false)
  else
    map:obstacle_set_group_enabled("n_water_off_obstacle", false)
  end

end

-- Called when the opening transition of the map finished
function map:on_map_opening_transition_finished(destination_point_name)

  -- show the welcome message
  if destination_point_name == "from_outside" then
    map:dialog_start("dungeon_3")
  end
end

function map:on_update()

  if not map:get_game():get_boolean(113)
    and are_all_torches_on() then

    lock_torches()
    map:camera_move(584, 360, 250, function()
      sol.audio.play_sound("secret")
      map:door_open("torches_door")
    end)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "se_door_switch" and not map:door_is_open("se_door") then
    map:camera_move(800, 728, 250, open_se_door)
  elseif switch_name == "ce_door_switch" and not map:door_is_open("ce_door") then
    map:camera_move(736, 552, 250, open_ce_door)
  elseif switch_name == "c_water_switch" and not map:get_game():get_boolean(121) then
    map:camera_move(344, 736, 250, remove_c_water, 1000, 3500)
  end
end

function open_se_door()
  sol.audio.play_sound("secret")
  map:door_open("se_door")
end

function open_ce_door()
  sol.audio.play_sound("secret")
  map:door_open("ce_door")
end

function remove_c_water()
  sol.audio.play_sound("water_drain_begin")
  sol.audio.play_sound("water_drain")
  map:tile_set_enabled("c_water_out", true)
  map:tile_set_enabled("c_water_source", false)
  sol.timer.start(remove_water_delay, remove_c_water_2)
end

function remove_c_water_2()
  map:tile_set_enabled("c_water_middle", false)
  sol.timer.start(remove_water_delay, remove_c_water_3)
end

function remove_c_water_3()
  map:tile_set_enabled("c_water", false)
  map:tile_set_enabled("c_water_less_1", true)
  sol.timer.start(remove_water_delay, remove_c_water_4)
end

function remove_c_water_4()
  map:tile_set_enabled("c_water_less_1", false)
  map:tile_set_enabled("c_water_less_2", true)
  sol.timer.start(remove_water_delay, remove_c_water_5)
end

function remove_c_water_5()
  map:tile_set_enabled("c_water_less_2", false)
  map:tile_set_enabled("c_water_less_3", true)
  sol.timer.start(remove_water_delay, remove_c_water_6)
end

function remove_c_water_6()
  map:tile_set_enabled("c_water_less_3", false)
  map:jumper_set_group_enabled("c_water_on_jumper", false)
  map:obstacle_set_group_enabled("c_water_off_obstacle", true)
  map:get_game():set_boolean(121, true)
  sol.audio.play_sound("secret")
end


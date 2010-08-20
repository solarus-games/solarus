-------------------------
-- Lyriann cave script --
-------------------------

tom_initial_x = 0
tom_initial_y = 0

function event_map_started(destination_point_name)

  tom_initial_x, tom_initial_y = npc_get_position("tom")

  if has_finished_cavern() and not has_boomerang_of_tom() then
    npc_remove("tom")
  end

  if savegame_get_boolean(38) then
    tile_set_enabled("barrier", false)
    switch_set_enabled("open_barrier_switch", true)
  end
end

function event_switch_enabled(switch_name)
  camera_move(136, 304, 10)
end

function event_camera_reached_target()
  if enemies_are_dead("battle_1") and tile_is_enabled("battle_1_barrier") then
    timer_start(1000, "battle_1_camera_timer", false)
  elseif enemies_are_dead("battle_2") and tile_is_enabled("battle_2_barrier") then
    timer_start(1000, "battle_2_camera_timer", false)
  elseif tile_is_enabled("barrier") then
    timer_start(1000, "camera_1_timer", false)
  end
end

function camera_1_timer()
  play_sound("secret")
  tile_set_enabled("barrier", false)
  savegame_set_boolean(38, true)
  timer_start(1000, "camera_restore", false)
end

function battle_1_camera_timer()
  play_sound("secret")
  tile_set_enabled("battle_1_barrier", false)
  timer_start(1000, "camera_restore", false)
end


function battle_2_camera_timer()
  play_sound("secret")
  tile_set_enabled("battle_2_barrier", false)
  timer_start(1000, "camera_restore", false)
end

function event_npc_dialog(npc)

  if npc == "tom" then
    if not has_seen_tom() then
      dialog_start("lyriann_cave.tom.first_time")
    elseif has_finished_cavern() then
      if has_boomerang_of_tom() then
        dialog_start("lyriann_cave.tom.cavern_finished")
      else
	dialog_start("lyriann_cave.tom.see_you_later")
      end
    elseif has_boomerang_of_tom() then
      dialog_start("lyriann_cave.tom.not_finished")
    else
      dialog_start("lyriann_cave.tom.not_first_time")
    end
  end
end

function event_dialog_finished(message_id, answer)

  if message_id == "lyriann_cave.tom.first_time" or message_id == "lyriann_cave.tom.not_first_time" then
    savegame_set_boolean(47, true)
    if answer == 0 then
      dialog_start("lyriann_cave.tom.accept_help")
    end
  elseif message_id == "lyriann_cave.tom.accept_help" then
    treasure_give(4, 41)
  elseif message_id == "lyriann_cave.tom.leaving" then
    play_sound("warp")
    hero_set_direction(1)
    timer_start(1700, "start_moving_tom", false)
  elseif message_id == "lyriann_cave.tom.not_finished" and answer == 1 then
    give_boomerang_back()
    dialog_start("lyriann_cave.tom.gave_boomerang_back")
  elseif message_id == "lyriann_cave.tom.cavern_finished"
    or message_id == "lyriann_cave.tom.leaving.cavern_not_finished"
    or message_id == "lyriann_cave.tom.leaving.cavern_finished" then

    give_boomerang_back()
    x,y = npc_get_position("tom")
    if y ~= tom_initial_y then
      npc_walk("tom", "2222220000002222222222222222", false, false)
    end
  end

end

function give_boomerang_back()
  inventory_item_set(3, 0)
  savegame_set_boolean(41, false)
end

function start_moving_tom()
  npc_set_position("tom", 88, 509)
  npc_walk("tom", "0000666666", false, false)
end

function event_npc_movement_finished(npc)

  if has_boomerang_of_tom() then
    if has_finished_cavern() then
      dialog_start("lyriann_cave.tom.cavern_finished")
    else
      dialog_start("lyriann_cave.tom.leaving.cavern_not_finished")
    end
  else
    npc_set_position("tom", tom_initial_x, tom_initial_y)
    npc_set_direction("tom", 3)
    hero_unfreeze()
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "leave_cavern_sensor" and has_boomerang_of_tom() then
      hero_freeze()
      dialog_start("lyriann_cave.tom.leaving")
  end
end

function has_seen_tom()
  return savegame_get_boolean(47)
end

function has_boomerang_of_tom()
  return savegame_get_boolean(41)
end

function has_gave_back_boomerang()
  return savegame_get_boolean(42)
end

function has_finished_cavern()
  -- the cavern is considered has finished if the player has found the heart container
  return savegame_get_boolean(37)
end

function event_enemy_dead(enemy_name)
  if enemies_are_dead("battle_1") and tile_is_enabled("battle_1_barrier") then
    camera_move(352, 288, 10)
  end
  if enemies_are_dead("battle_2") and tile_is_enabled("battle_2_barrier") then
    camera_move(344, 488, 10)
  end
end


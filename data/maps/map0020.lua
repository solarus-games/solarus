-------------------------
-- Lyriann cave script --
-------------------------

tom_initial_x = 0
tom_initial_y = 0

function event_map_started()

  tom_initial_x, tom_initial_y = npc_get_position("tom")

  if has_finished_cavern() then
    npc_remove("tom")
  end

  if savegame_get_boolean(38) then
    disable_tile("barrier")
    switch_set_enabled("open_barrier_switch", true)
  end
end

function event_switch_enabled(switch_name)
  move_camera(136, 304, 10)
end

function event_camera_reached_target()
  start_timer(1000, "camera_timer", false)
end

function camera_timer()
  play_sound("secret")
  disable_tile("barrier")
  savegame_set_boolean(38, true)
  start_timer(1000, "restore_camera", false)
end

function event_npc_dialog(npc)

  if npc == "tom" then
    if not has_seen_tom() then
      start_message("lyriann_cave.tom.first_time")
    elseif has_finished_cavern() then
      if has_boomerang_of_tom() then
        start_message("lyriann_cave.tom.cavern_finished")
      else
	start_message("lyriann_cave.tom.see_you_later")
      end
    elseif has_boomerang_of_tom() then
      start_message("lyriann_cave.tom.not_finished")
    else
      start_message("lyriann_cave.tom.not_first_time")
    end
  end
end

function event_message_sequence_finished(message_id, answer)

  if message_id == "lyriann_cave.tom.first_time" or message_id == "lyriann_cave.tom.not_first_time" then
    savegame_set_boolean(47, true)
    if answer == 0 then
      start_message("lyriann_cave.tom.accept_help")
    end
  elseif message_id == "lyriann_cave.tom.accept_help" then
    give_treasure(4, 41)
  elseif message_id == "lyriann_cave.tom.leaving" then
    play_sound("warp")
    hero_set_direction(1)
    start_timer(1700, "start_moving_tom", false)
  elseif message_id == "lyriann_cave.tom.not_finished" and answer == 1 then
    give_boomerang_back()
    start_message("lyriann_cave.tom.gave_boomerang_back")
  elseif message_id == "lyriann_cave.tom.leaving.cavern_not_finished"
    or message_id == "lyriann_cave.tom.cavern_finished" then
    give_boomerang_back()
    npc_walk("tom", "22222200000022222222222222", false, true)
  end

end

function give_boomerang_back()
  inventory_item_set(3, 0)
  savegame_set_boolean(41, false)
end

function start_moving_tom()
  npc_set_position("tom", 88, 509)
  npc_walk("tom", "0000666666", false, true)
end

function event_npc_movement_finished(npc)

  if has_boomerang_of_tom() then
    if has_finished_cavern() then
      start_message("lyriann_cave.tom.leaving.cavern_finished")
    else
      start_message("lyriann_cave.tom.leaving.cavern_not_finished")
    end
  else
    npc_set_position("tom", tom_initial_x, tom_initial_y)
    npc_set_direction("tom", 3)
    unfreeze()
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "leave_cavern_sensor" and has_boomerang_of_tom() then
      freeze()
      start_message("lyriann_cave.tom.leaving")
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


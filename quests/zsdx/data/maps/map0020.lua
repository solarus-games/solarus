-- Lyriann cave 1F

tom_initial_x = 0
tom_initial_y = 0
tom_sprite = nil

function event_map_started(destination_point_name)

  tom_initial_x, tom_initial_y = sol.map.npc_get_position("tom")

  if has_finished_cavern() and not has_boomerang_of_tom() then
    sol.map.npc_remove("tom")
  else
    tom_sprite = sol.map.npc_get_sprite("tom")
  end

  if sol.map.get_game():get_boolean(38) then
    sol.map.tile_set_enabled("barrier", false)
    sol.map.switch_set_activated("open_barrier_switch", true)
  end
end

function event_switch_activated(switch_name)
  sol.map.camera_move(136, 304, 250, camera_1_timer)
end

function camera_1_timer()
  sol.audio.play_sound("secret")
  sol.map.tile_set_enabled("barrier", false)
  sol.map.get_game():set_boolean(38, true)
end

function battle_1_camera_timer()
  sol.audio.play_sound("secret")
  sol.map.tile_set_enabled("battle_1_barrier", false)
end


function battle_2_camera_timer()
  sol.audio.play_sound("secret")
  sol.map.tile_set_enabled("battle_2_barrier", false)
end

function event_npc_interaction(npc)

  if npc == "tom" then
    if not has_seen_tom() then
      sol.map.dialog_start("lyriann_cave.tom.first_time")
    elseif has_finished_cavern() then
      if has_boomerang_of_tom() then
        sol.map.dialog_start("lyriann_cave.tom.cavern_finished")
      else
	sol.map.dialog_start("lyriann_cave.tom.see_you_later")
      end
    elseif has_boomerang_of_tom() then
      sol.map.dialog_start("lyriann_cave.tom.not_finished")
    else
      sol.map.dialog_start("lyriann_cave.tom.not_first_time")
    end
  end
end

function event_dialog_finished(message_id, answer)

  if message_id == "lyriann_cave.tom.first_time" or message_id == "lyriann_cave.tom.not_first_time" then
    sol.map.get_game():set_boolean(47, true)
    if answer == 0 then
      sol.map.dialog_start("lyriann_cave.tom.accept_help")
    end
  elseif message_id == "lyriann_cave.tom.accept_help" then
    sol.map.treasure_give("boomerang", 1, 41)
  elseif message_id == "lyriann_cave.tom.leaving" then
    sol.audio.play_sound("warp")
    sol.map.hero_set_direction(1)
    sol.timer.start(1700, start_moving_tom)
  elseif message_id == "lyriann_cave.tom.not_finished" and answer == 1 then
    give_boomerang_back()
    sol.map.dialog_start("lyriann_cave.tom.gave_boomerang_back")
  elseif message_id == "lyriann_cave.tom.cavern_finished"
    or message_id == "lyriann_cave.tom.leaving.cavern_not_finished"
    or message_id == "lyriann_cave.tom.leaving.cavern_finished" then

    give_boomerang_back()
    local x, y = sol.map.npc_get_position("tom")
    if y ~= tom_initial_y then
      local m = sol.movement.create("path")
      m:set_path{2,2,2,2,2,2,0,0,0,0,0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2}
      m:set_speed(48)
      sol.map.npc_start_movement("tom", m)
      tom_sprite:set_animation("walking")
    end
  end

end

function give_boomerang_back()
  sol.map.get_game():set_item("boomerang", 0)
  sol.map.get_game():set_boolean(41, false)
end

function start_moving_tom()
  local m = sol.movement.create("path")
  m:set_path{0,0,0,0,6,6,6,6,6,6}
  m:set_speed(48)
  sol.map.npc_set_position("tom", 88, 509)
  sol.map.npc_start_movement("tom", m)
  tom_sprite:set_animation("walking")
end

function event_npc_movement_finished(npc)

  if has_boomerang_of_tom() then
    if has_finished_cavern() then
      sol.map.dialog_start("lyriann_cave.tom.cavern_finished")
    else
      sol.map.dialog_start("lyriann_cave.tom.leaving.cavern_not_finished")
    end
  else
    sol.map.npc_set_position("tom", tom_initial_x, tom_initial_y)
    tom_sprite:set_direction(3)
    sol.map.hero_unfreeze()
  end
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "leave_cavern_sensor" and has_boomerang_of_tom() then
    sol.map.hero_freeze()
    sol.map.dialog_start("lyriann_cave.tom.leaving")
  end
end

function has_seen_tom()
  return sol.map.get_game():get_boolean(47)
end

function has_boomerang_of_tom()
  return sol.map.get_game():get_boolean(41)
end

function has_finished_cavern()
  -- the cavern is considered has finished if the player has found the heart container
  return sol.map.get_game():get_boolean(37)
end

function event_enemy_dead(enemy_name)
  if sol.map.enemy_is_group_dead("battle_1") and sol.map.tile_is_enabled("battle_1_barrier") then
    sol.map.camera_move(352, 288, 250, battle_1_camera_timer)
  end
  if sol.map.enemy_is_group_dead("battle_2") and sol.map.tile_is_enabled("battle_2_barrier") then
    sol.map.camera_move(344, 488, 250, battle_2_camera_timer)
  end
end


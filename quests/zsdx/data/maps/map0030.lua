-------------------------
-- Dungeon 2 1F script --
-------------------------

current_switch = ""

function event_map_started(destination_point_name)

  -- west barrier
  if sol.game.savegame_get_boolean(78) then
    sol.map.tile_set_enabled("barrier", false)
    sol.map.switch_set_enabled("barrier_switch", true)
  end

  -- hidden stairs
  if sol.game.savegame_get_boolean(90) then
    open_hidden_stairs()
  end

  -- hidden door
  if sol.game.savegame_get_boolean(91) then
    open_hidden_door()
  end

  -- miniboss doors
  sol.map.door_set_open("stairs_door", true)
  sol.map.door_set_open("miniboss_door", true)
end

fighting_miniboss = false
function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor" and not sol.game.savegame_get_boolean(92) and not fighting_miniboss then
    -- the miniboss is alive
    sol.map.door_close("miniboss_door")
    sol.map.hero_freeze()
    sol.main.timer_start(1000, "miniboss_timer", false)
    fighting_miniboss = true
  end
end

function miniboss_timer()
  sol.map.enemy_start_miniboss("miniboss")
  sol.map.hero_unfreeze()
end

function event_enemy_dead(enemy_name)

  if enemy_name == "miniboss" then
    sol.map.enemy_end_miniboss()
    sol.map.door_open("miniboss_door")
  end
end

function event_switch_enabled(switch_name)

  current_switch = switch_name
  if switch_name == "barrier_switch" then
    sol.map.camera_move(120, 536, 15)
  elseif switch_name == "pegasus_run_switch" then
    sol.map.switch_set_enabled("pegasus_run_switch_2", true)
    sol.map.camera_move(904, 88, 30)
  elseif switch_name == "pegasus_run_switch_2" then
    sol.main.play_sound("door_open")
    sol.map.tile_set_enabled("pegasus_run_barrier", false)
    sol.map.switch_set_enabled("pegasus_run_switch", true)
  elseif switch_name == "left_eye_switch" then
    check_eye_statues()
  elseif switch_name == "right_eye_switch" then
    check_eye_statues()
  end
end

function event_camera_reached_target()

  if current_switch == "barrier_switch" then
    sol.main.timer_start(1000, "barrier_camera_timer", false)
  elseif current_switch == "pegasus_run_switch" then
    sol.main.timer_start(1000, "pegasus_run_camera_timer", false)
  elseif not sol.game.savegame_get_boolean(90) then
    sol.main.timer_start(1000, "hidden_stairs_timer", false)
  else
    sol.main.timer_start(1000, "hidden_door_timer", false)
  end
end

function event_camera_back()
  if current_switch == "pegasus_run_switch" then
    sol.main.timer_start(7000, "pegasus_run_timer", true)
  end
end

function check_eye_statues()

  if sol.map.switch_is_enabled("left_eye_switch") and switch_is_enabled("right_eye_switch") then

    sol.map.switch_set_enabled("left_eye_switch", false)
    sol.map.switch_set_enabled("right_eye_switch", false)

    if not sol.game.savegame_get_boolean(90) then
      sol.main.play_sound("switch")
      sol.map.camera_move(456, 232, 15)
    elseif not sol.game.savegame_get_boolean(91) then
      sol.main.play_sound("switch")
      sol.map.camera_move(520, 320, 15)
    end
  end
end

function barrier_camera_timer()
  sol.main.play_sound("secret")
  sol.map.tile_set_enabled("barrier", false)
  sol.game.savegame_set_boolean(78, true)
  sol.main.timer_start(1000, "camera_restore", false)
end

function pegasus_run_camera_timer()
  sol.main.play_sound("secret")
  sol.map.tile_set_enabled("pegasus_run_barrier", false)
  sol.main.timer_start(1000, "camera_restore", false)
end

function pegasus_run_timer()
  sol.main.play_sound("door_closed")
  sol.map.tile_set_enabled("pegasus_run_barrier", true)
  sol.map.switch_set_enabled("pegasus_run_switch", false)
  sol.map.switch_set_enabled("pegasus_run_switch_2", false)
end

function hidden_stairs_timer()
  sol.main.play_sound("secret")
  open_hidden_stairs()
  sol.game.savegame_set_boolean(90, true)
  sol.main.timer_start(1000, "camera_restore", false)
end

function hidden_door_timer()
  sol.main.play_sound("secret")
  open_hidden_door()
  sol.game.savegame_set_boolean(91, true)
  sol.main.timer_start(1000, "camera_restore", false)
end

function open_hidden_stairs()
  sol.map.tile_set_group_enabled("hidden_stairs_closed", false)
  sol.map.tile_set_group_enabled("hidden_stairs_open", true)
end

function open_hidden_door()
  sol.map.tile_set_group_enabled("hidden_door_closed", false)
  sol.map.tile_set_group_enabled("hidden_door_open", true)
end


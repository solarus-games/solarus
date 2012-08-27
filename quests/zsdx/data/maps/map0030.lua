local map = ...
-- Dungeon 2 1F

current_switch = nil
fighting_miniboss = false

function map:on_started(destination_point)

  -- west barrier
  if map:get_game():get_boolean(78) then
    barrier:set_enabled(false)
    barrier_switch:set_activated(true)
  end

  -- hidden stairs
  if map:get_game():get_boolean(90) then
    open_hidden_stairs()
  end

  -- hidden door
  if map:get_game():get_boolean(91) then
    open_hidden_door()
  end

  -- miniboss doors
  map:set_doors_open("stairs_door", true)
  map:set_doors_open("miniboss_door", true)
end

function map:on_opening_transition_finished(destination_point)

  -- show the welcome message
  if destination_point:get_name() == "from_outside" then
    map:start_dialog("dungeon_2")
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor" and not map:get_game():get_boolean(92) and not fighting_miniboss then
    -- the miniboss is alive
    map:close_doors("miniboss_door")
    map:get_hero():freeze()
    sol.timer.start(1000, miniboss_timer)
    fighting_miniboss = true
  end
end

function miniboss_timer()
  sol.audio.play_music("boss")
  miniboss:set_enabled(true)
  map:get_hero():unfreeze()
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "miniboss" then
    sol.audio.play_music("light_world_dungeon")
    map:open_doors("miniboss_door")
  end
end

function map:on_switch_activated(switch_name)

  current_switch = switch_name

  if switch_name == "barrier_switch" then
    map:move_camera(120, 536, 250, barrier_camera_timer)
  elseif switch_name == "pegasus_run_switch" then
    pegasus_run_switch_2:set_activated(true)
    map:move_camera(904, 88, 250, pegasus_run_camera_timer)
  elseif switch_name == "pegasus_run_switch_2" then
    sol.audio.play_sound("door_open")
    pegasus_run_barrier:set_enabled(false)
    pegasus_run_switch:set_activated(true)
  elseif switch_name == "left_eye_switch" then
    check_eye_statues()
  elseif switch_name == "right_eye_switch" then
    check_eye_statues()
  end
end

function map:on_camera_back()

  if current_switch == "pegasus_run_switch" then
    sol.timer.start(7000, true, pegasus_run_timer)
  end
end

function check_eye_statues()

  if left_eye_switch:is_activated() and right_eye_switch:is_activated() then

    left_eye_switch:set_activated(false)
    right_eye_switch:set_activated(false)

    if not map:get_game():get_boolean(90) then
      sol.audio.play_sound("switch")
      map:move_camera(456, 232, 250, hidden_stairs_timer)
    elseif not map:get_game():get_boolean(91) then
      sol.audio.play_sound("switch")
      map:move_camera(520, 320, 250, hidden_door_timer)
    end
  end
end

function barrier_camera_timer()
  sol.audio.play_sound("secret")
  barrier:set_enabled(false)
  map:get_game():set_boolean(78, true)
end

function pegasus_run_camera_timer()
  sol.audio.play_sound("secret")
  pegasus_run_barrier:set_enabled(false)
end

function pegasus_run_timer()
  sol.audio.play_sound("door_closed")
  pegasus_run_barrier:set_enabled(true)
  pegasus_run_switch:set_activated(false)
  pegasus_run_switch_2:set_activated(false)
end

function hidden_stairs_timer()
  sol.audio.play_sound("secret")
  open_hidden_stairs()
  map:get_game():set_boolean(90, true)
end

function hidden_door_timer()
  sol.audio.play_sound("secret")
  open_hidden_door()
  map:get_game():set_boolean(91, true)
end

function open_hidden_stairs()
  map:set_entities_enabled("hidden_stairs_closed", false)
  map:set_entities_enabled("hidden_stairs_open", true)
end

function open_hidden_door()
  map:set_entities_enabled("hidden_door_closed", false)
  map:set_entities_enabled("hidden_door_open", true)
end


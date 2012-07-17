-- Dungeon 6 3F

fighting_miniboss = false

function event_map_started(destination_point_name)

  -- game ending sequence
  if destination_point_name == "from_ending" then
    sol.map.hero_freeze()
    sol.map.hero_set_visible(false)
    sol.map.hud_set_enabled(false)
    sol.map.enemy_set_group_enabled("", false)
    sol.audio.play_music("fanfare")
  end

  sol.map.door_set_open("miniboss_door", true)
  sol.map.enemy_set_group_enabled("miniboss", false)
  if sol.map.get_game():get_boolean(320) then
    sol.map.tile_set_group_enabled("miniboss_fake_floor", false)
  end

  if sol.map.get_game():get_boolean(323) then
    lock_torches()
  end
end

function event_map_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_ending" then
    sol.map.dialog_start("credits_3")
    sol.map.camera_move(120, 408, 25, function() end, 1e6)
  end
end

function are_all_torches_on()

  return sol.map.npc_exists("torch_1")
      and sol.map.npc_get_sprite("torch_1"):get_animation() == "lit"
      and sol.map.npc_get_sprite("torch_2"):get_animation() == "lit"
end

-- Makes all torches on forever
function lock_torches()
  sol.map.npc_remove("torch_1")
  sol.map.npc_remove("torch_2")
end

function event_update()

  if not sol.map.door_is_open("torches_door")
      and are_all_torches_on() then

    sol.map.camera_move(360, 104, 250, open_torches_door)
  end
end

function open_torches_door()

  sol.audio.play_sound("secret")
  sol.map.door_open("torches_door")
  lock_torches()
end

function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor"
      and not sol.map.get_game():get_boolean(320)
      and not fighting_miniboss then

    sol.map.hero_freeze()
    sol.map.door_close("miniboss_door")
    fighting_miniboss = true
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      sol.map.enemy_set_group_enabled("miniboss", true)
      sol.map.tile_set_group_enabled("miniboss_fake_floor", false)
      sol.map.hero_unfreeze()
    end)
  end
end

function event_enemy_dead(enemy_name)

  if string.find(enemy_name, "^miniboss")
      and sol.map.enemy_is_group_dead("miniboss") then

    sol.audio.play_music("dark_world_dungeon")
    sol.map.door_open("miniboss_door")
    sol.map.get_game():set_boolean(320, true)
  end
end

function event_dialog_finished(dialog_id)

  if dialog_id == "credits_3" then
   sol.timer.start(2000, ending_next)
  end
end

function ending_next()
  sol.map.hero_set_map(89, "from_ending", 1)
end


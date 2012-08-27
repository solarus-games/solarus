local map = ...
-- Dungeon 6 3F

fighting_miniboss = false

function map:on_started(destination_point)

  -- game ending sequence
  if destination_point:get_name() == "from_ending" then
    map:get_hero():freeze()
    map:get_hero():set_visible(false)
    map:get_game():set_hud_enabled(false)
    map:set_entities_enabled("", false)
    sol.audio.play_music("fanfare")
  end

  map:set_doors_open("miniboss_door", true)
  map:set_entities_enabled("miniboss", false)
  if map:get_game():get_boolean(320) then
    map:set_entities_enabled("miniboss_fake_floor", false)
  end

  if map:get_game():get_boolean(323) then
    lock_torches()
  end
end

function map:on_opening_transition_finished(destination_point)

  if destination_point:get_name() == "from_ending" then
    map:start_dialog("credits_3")
    map:move_camera(120, 408, 25, function() end, 1e6)
  end
end

function are_all_torches_on()

  return map:npc_exists("torch_1")
      and torch_1:get_sprite():get_animation() == "lit"
      and torch_2:get_sprite():get_animation() == "lit"
end

-- Makes all torches on forever
function lock_torches()
  torch_1:remove()
  torch_2:remove()
end

function map:on_update()

  if not torches_door:is_open()
      and are_all_torches_on() then

    map:move_camera(360, 104, 250, open_torches_door)
  end
end

function open_torches_door()

  sol.audio.play_sound("secret")
  map:open_doors("torches_door")
  lock_torches()
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor"
      and not map:get_game():get_boolean(320)
      and not fighting_miniboss then

    map:get_hero():freeze()
    map:close_doors("miniboss_door")
    fighting_miniboss = true
    sol.timer.start(1000, function()
      sol.audio.play_music("boss")
      map:set_entities_enabled("miniboss", true)
      map:set_entities_enabled("miniboss_fake_floor", false)
      map:get_hero():unfreeze()
    end)
  end
end

function map:on_enemy_dead(enemy_name)

  if string.find(enemy_name, "^miniboss")
      and not map:has_entities("miniboss") then

    sol.audio.play_music("dark_world_dungeon")
    map:open_doors("miniboss_door")
    map:get_game():set_boolean(320, true)
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "credits_3" then
   sol.timer.start(2000, ending_next)
  end
end

function ending_next()
  map:get_hero():teleport(89, "from_ending")
end


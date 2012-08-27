local map = ...
-- Dungeon 1 2F

fighting_miniboss = false

function map:on_started(destination_point)

  boss_key_chest:set_enabled(false)
  map:set_doors_open("stairs_door", true)
  map:set_doors_open("miniboss_door", true)
end

function map:on_opening_transition_finished(destination_point)

  -- show the welcome message
  if destination_point:get_name() == "from_outside" then
    map:start_dialog("dungeon_1")
  end
end

function map:on_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor" and not map:get_game():get_boolean(62) and not fighting_miniboss then
    -- the miniboss is alive
    map:close_doors("miniboss_door")
    map:get_hero():freeze()
    sol.timer.start(1000, miniboss_timer)
    fighting_miniboss = true
  end
end

function miniboss_timer()
  sol.audio.play_music("boss")
  khorneth:set_enabled(true)
  map:get_hero():unfreeze()
end

function map:on_enemy_dead(enemy_name)

  if enemy_name == "khorneth" then
    sol.audio.play_music("light_world_dungeon")
    map:open_doors("miniboss_door")
  end

  if not map:has_entities("boss_key_battle")
      and not boss_key_chest:is_enabled() then
    map:move_camera(104, 72, 250, boss_key_timer)
  end
end

function boss_key_timer()
  sol.audio.play_sound("chest_appears")
  boss_key_chest:set_enabled(true)
end


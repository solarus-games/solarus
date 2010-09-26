-------------------------
-- Dungeon 1 2F script --
-------------------------

function event_map_started(destination_point_name)

  chest_set_hidden("boss_key_chest", true)
  door_set_open("stairs_door", true)
  door_set_open("miniboss_door", true)
end

fighting_miniboss = false
function event_hero_on_sensor(sensor_name)

  if sensor_name == "start_miniboss_sensor" and not savegame_get_boolean(62) and not fighting_miniboss then
    -- the miniboss is alive
    door_close("miniboss_door")
    hero_freeze()
    timer_start(1000, "miniboss_timer", false)
    fighting_miniboss = true
  end
end

function miniboss_timer()
  enemy_start_miniboss("khorneth")
  hero_unfreeze()
end

function event_enemy_dead(enemy_name)

  if enemy_name == "khorneth" then
    enemy_end_miniboss()
    door_open("miniboss_door")
  end

  if enemy_is_group_dead("boss_key_battle") and chest_is_hidden("boss_key_chest") then
    camera_move(104, 72, 15)
  end
end

function event_camera_reached_target()
  timer_start(1000, "boss_key_timer", false)
end

function boss_key_timer()
  play_sound("chest_appears")
  chest_set_hidden("boss_key_chest", false)
  timer_start(1000, "camera_restore", false)
end


-------------------------
-- Dungeon 1 2F script --
-------------------------

function event_map_started(destination_point_name)
  if not savegame_get_boolean(57) then
    chest_set_hidden("boss_key_chest", true)
  end

  door_set_open("stairs_door", true)
  door_set_open("miniboss_door", true)
end

function event_opening_transition_finished(destination_point_name)

  if destination_point_name == "from_1F" and not savegame_get_boolean(62) then
    -- the miniboss is alive
    door_close("miniboss_door")
    freeze()
    start_timer(1000, "miniboss_timer", false)
  end
end

function miniboss_timer()
  miniboss_start_battle("khorneth")
  unfreeze()
end

function event_enemy_dead(enemy_name)

  if enemy_name == "khorneth" then
    miniboss_end_battle()
    door_open("miniboss_door")
  end

  if enemies_are_dead("boss_key_battle") and chest_is_hidden("boss_key_chest") then
    move_camera(104, 72, 15)
  end
end

function event_camera_reached_target()
  start_timer(1000, "boss_key_timer", false)
end

function boss_key_timer()
  play_sound("chest_appears")
  chest_set_hidden("boss_key_chest", false)
  start_timer(1000, "restore_camera", false)
end


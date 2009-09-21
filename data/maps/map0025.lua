-------------------------
-- Dungeon 1 2F script --
-------------------------

function event_map_started(destination_point_name)
  if not savegame_get_boolean(57) then
    chest_set_hidden("boss_key_chest", true)
  end

  -- temporary
  if savegame_get_boolean(62) then
    switch_set_enabled("switch", true)
  end
  door_set_open("stairs_door", true)
end

function event_enemy_dead(enemy_name)
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

-- temporary
function event_switch_enabled(switch_name)
  door_open("miniboss_door")
end


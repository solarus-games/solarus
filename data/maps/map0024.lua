-------------------------
-- Dungeon 1 B1 script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_1F_hole" then
    -- we are in the boss room

    if savegame_get_boolean(63) and savegame_get_boolean(64) then
      -- the boss was already killed and the heart container was also picked:
      -- make the hero leave the dungeon since the room is closed
      freeze()
      start_timer(1000, "boss_already_killed", false)
    else
      -- normal case
      boss_start_battle("boss")
    end
  end
end

function boss_already_killed()
  -- leave the dungeon
  play_sound("warp")
  hero_set_map(6, "from_dungeon_1_1F", 1)
end

function event_obtained_treasure(content, savegame_variable)
  if content == 80 then
    boss_end_battle()
    start_timer(9000, "start_final_room", false);
  end
end

function event_switch_enabled(switch_name)

  if switch_name == "se_switch" or switch_name == "ne_switch" then
    move_camera(960, 496, 15)
  end
end

function event_camera_reached_target()
  start_timer(1000, "se_room_timer", false)
end

function se_room_timer()
  open_se_door()
  start_timer(1000, "restore_camera", false)
end

function open_se_door()
  play_sound("secret")
  door_open("se_door")
  switch_set_enabled("se_switch", true)
  switch_set_enabled("ne_switch", true)
end

function start_final_room()
  hero_set_map(27, "from_boss", 1);
end


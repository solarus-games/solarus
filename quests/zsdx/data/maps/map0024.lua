-------------------------
-- Dungeon 1 B1 script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_1F_hole" then
    -- we are in the boss room

    if savegame_get_boolean(63) and savegame_get_boolean(64) then
      -- the boss was already killed and the heart container was also picked:
      -- make the hero leave the map since the room is closed
      if not equipment_is_dungeon_finished(1) then
	start_final_room()
      else
        hero_freeze()
        timer_start(1000, "boss_already_killed", false)
      end
    else
      -- normal case
      enemy_start_boss("boss")
    end
  end
end

function boss_already_killed()
  -- leave the dungeon
  play_sound("warp")
  hero_set_map(6, "from_dungeon_1_1F", 1)
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    enemy_end_boss()
    timer_start(9000, "start_final_room", false);
  end
end

function event_switch_enabled(switch_name)
  if switch_name == "se_switch" or switch_name == "ne_switch" then
    camera_move(960, 496, 15)
  end
end

function event_camera_reached_target()
  timer_start(1000, "se_room_timer", false)
end

function se_room_timer()
  open_se_door()
  timer_start(1000, "camera_restore", false)
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


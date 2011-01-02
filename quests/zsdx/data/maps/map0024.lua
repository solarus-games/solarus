-------------------------
-- Dungeon 1 B1 script --
-------------------------

function event_map_started(destination_point_name)

  if destination_point_name == "from_1F_hole" then
    -- we are in the boss room

    if sol.game.savegame_get_boolean(63) and sol.game.savegame_get_boolean(64) then
      -- the boss was already killed and the heart container was also picked:
      -- make the hero leave the map since the room is closed
      if not sol.game.is_dungeon_finished(1) then
	start_final_room()
      else
        sol.map.hero_freeze()
        sol.main.timer_start(1000, "boss_already_killed", false)
      end
    else
      -- normal case
      sol.map.enemy_start_boss("boss")
    end
  end
end

function boss_already_killed()
  -- leave the dungeon
  sol.main.play_sound("warp")
  sol.map.hero_set_map(6, "from_dungeon_1_1F", 1)
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.map.enemy_end_boss()
    sol.main.timer_start(9000, "start_final_room", false);
  end
end

function event_switch_activated(switch_name)
  if switch_name == "se_switch" or switch_name == "ne_switch" then
    sol.map.camera_move(960, 496, 150)
  end
end

function event_camera_reached_target()
  sol.main.timer_start(1000, "se_room_timer", false)
end

function se_room_timer()
  open_se_door()
  sol.main.timer_start(1000, "sol.map.camera_restore", false)
end

function open_se_door()
  sol.main.play_sound("secret")
  sol.map.door_open("se_door")
  sol.map.switch_set_activated("se_switch", true)
  sol.map.switch_set_activated("ne_switch", true)
end

function start_final_room()
  sol.map.hero_set_map(27, "from_boss", 1);
end


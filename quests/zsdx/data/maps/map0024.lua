-- Dungeon 1 B1

function event_map_started(destination_point_name)

  if destination_point_name == "from_1F_hole" then
    -- we are in the boss room

    if sol.game.savegame_get_boolean(63) then
      -- the boss is already dead

      if sol.game.savegame_get_boolean(64) then
        -- the heart container was also picked: open the final room door
        sol.map.door_set_open("final_room_door", true)
      end
    else
      -- normal case
      start_boss()
    end
  end
end

function start_boss()

  sol.map.enemy_set_enabled("boss", true)
  sol.main.play_music("boss.spc")
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.main.timer_start(9000, "open_final_room", false)
    sol.main.play_music("victory.spc")
    sol.map.hero_freeze()
    sol.map.hero_set_direction(3)
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

function open_final_room()
  sol.map.door_open("final_room_door")
  sol.main.play_sound("secret")
  sol.map.hero_unfreeze()
end


local map = ...
-- Dungeon 1 B1

function map:on_started(destination_point_name)

  if destination_point_name == "from_1F_hole" then
    -- we are in the boss room

    if map:get_game():get_boolean(63) then
      -- the boss is already dead

      if map:get_game():get_boolean(64) then
        -- the heart container was also picked: open the final room door
        map:door_set_open("final_room_door", true)
      end
    else
      -- normal case
      start_boss()
    end
  elseif destination_point_name == "from_1F_east" then
    map:door_set_open("se_door", true)
    map:switch_set_activated("se_switch", true)
  end
end

function start_boss()

  map:enemy_set_enabled("boss", true)
  sol.audio.play_music("boss")
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.timer.start(9000, open_final_room)
    sol.audio.play_music("victory")
    map:hero_freeze()
    map:hero_set_direction(3)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "se_switch" then
    map:camera_move(960, 496, 250, open_se_door)
  end
end

function open_se_door()

  sol.audio.play_sound("secret")
  map:door_open("se_door")
  map:switch_set_activated("se_switch", true)
end

function open_final_room()

  map:door_open("final_room_door")
  sol.audio.play_sound("secret")
  map:hero_unfreeze()
end


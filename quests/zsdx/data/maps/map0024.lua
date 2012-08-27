local map = ...
-- Dungeon 1 B1

function map:on_started(destination_point)

  if destination_point:get_name() == "from_1F_hole" then
    -- we are in the boss room

    if map:get_game():get_boolean(63) then
      -- the boss is already dead

      if map:get_game():get_boolean(64) then
        -- the heart container was also picked: open the final room door
        map:set_doors_open("final_room_door", true)
      end
    else
      -- normal case
      start_boss()
    end
  elseif destination_point:get_name() == "from_1F_east" then
    map:set_doors_open("se_door", true)
    se_switch:set_activated(true)
  end
end

function start_boss()

  boss:set_enabled(true)
  sol.audio.play_music("boss")
end

function map:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.timer.start(9000, open_final_room)
    sol.audio.play_music("victory")
    map:get_hero():freeze()
    map:get_hero():set_direction(3)
  end
end

function map:on_switch_activated(switch_name)

  if switch_name == "se_switch" then
    map:move_camera(960, 496, 250, open_se_door)
  end
end

function open_se_door()

  sol.audio.play_sound("secret")
  map:open_doors("se_door")
  se_switch:set_activated(true)
end

function open_final_room()

  map:open_doors("final_room_door")
  sol.audio.play_sound("secret")
  map:get_hero():unfreeze()
end


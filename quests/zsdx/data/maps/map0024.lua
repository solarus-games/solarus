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
      boss:set_enabled(true)
      sol.audio.play_music("boss")
    end
  elseif destination_point:get_name() == "from_1F_east" then
    map:set_doors_open("se_door", true)
    se_switch:set_activated(true)
  end
end

function hero:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "heart_container" then
    sol.timer.start(9000, function()
      map:open_doors("final_room_door")
      sol.audio.play_sound("secret")
      hero:unfreeze()
    end)
    sol.audio.play_music("victory")
    self:freeze()
    self:set_direction(3)
  end
end

function se_switch:on_activated()

  map:move_camera(960, 496, 250, function()
    sol.audio.play_sound("secret")
    map:open_doors("se_door")
    se_switch:set_activated(true)
  end)
end


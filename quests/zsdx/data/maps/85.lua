local map = ...
-- Dungeon 3 final room

function map:on_opening_transition_finished(destination)
  local solarus_child_sprite = solarus_child:get_sprite()
  solarus_child:set_position(160, 165)
  solarus_child_sprite:set_animation("stopped")
  solarus_child_sprite:set_ignore_suspend(true)
end

function solarus_child:on_interaction()

  if map:get_game():is_dungeon_finished(3) then
    -- dialog already done
    sol.audio.play_sound("warp")
    hero:teleport(3, "from_dungeon_3")
  else
    -- start the final sequence
    map:move_camera(160, 120, 100, function()
      map:set_dialog_variable("dungeon_3.solarus_child", map:get_game():get_player_name())
      map:start_dialog("dungeon_3.solarus_child", function()
        hero:start_victory()
      end)
    end, 0)
  end
end

function hero:on_victory_finished()
  map:get_game():set_dungeon_finished(3)
  hero:teleport(3, "from_dungeon_3")
end


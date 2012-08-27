local map = ...
-- Dungeon 1 final room

function map:on_opening_transition_finished(destination_point)
  local solarus_child_sprite = solarus_child:get_sprite()
  solarus_child:set_position(160, 165)
  solarus_child_sprite:set_animation("stopped")
  solarus_child_sprite:set_ignore_suspend(true)
end

function solarus_child:on_interaction()

  if map:get_game():is_dungeon_finished(1) then
    -- dialog already done
    sol.audio.play_sound("warp")
    hero:teleport(6, "from_dungeon_1_1F")
  else
    -- start the final sequence
    map:move_camera(160, 120, 100, function()
      map:set_dialog_variable("dungeon_1.solarus_child", map:get_game():get_player_name())
      map:start_dialog("dungeon_1.solarus_child", function()
        hero:start_victory()
      end)
    end)
  end
end

function hero:on_hero_victory_finished()
  map:get_game():set_dungeon_finished(1)
  self:teleport(6, "from_dungeon_1_1F")
  map:set_pause_enabled(true)
end


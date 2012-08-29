local map = ...
-- Outside world B4

function map:on_started(destination_point)

  -- enable dark world
  if map:get_game():get_boolean(905) then
    sol.audio.play_music("dark_world")
    map:set_tileset(13)
  else
    sol.audio.play_music("overworld")
  end

  -- don't allow to go to the surprise wall before dungeon 3 is finished
  if not map:get_game():is_dungeon_finished(3) then
    surprise_wall_guy:remove()
  end

  if map:get_game():get_boolean(136) then
    surprise_wall_door_tile:set_enabled(false)
    surprise_wall_door:remove()
  end
end

function surprise_wall_guy:on_interaction()

  if surprise_wall_door_tile:is_enabled() then
    map:start_dialog("outside_world.surprise_wall_guy.closed", function()
      if map:get_game():get_item("level_4_way").get_variant() == 1 then
        -- the player has the apple pie
        map:start_dialog("outside_world.surprise_wall_guy.give_me_apple_pie", function(answer)
          if answer == 0 then
            map:get_game():get_item("level_4_way"):set_variant(0)
            map:start_dialog("outside_world.surprise_wall_guy.thanks", function()
              surprise_wall_door_tile:set_enabled(false)
              surprise_wall_door:remove()
              map:get_game():set_boolean(136, true)
              sol.audio.play_sound("secret")
              sol.audio.play_sound("door_open")
            end)
          end
        end)
      end
    end)
  else
    map:start_dialog("outside_world.surprise_wall_guy.open")
  end
end

function surprise_wall_door:on_interaction()
  map:start_dialog("outside_world.surprise_wall_closed")
end


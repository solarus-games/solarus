-- Dungeon 9 1F

function event_map_started(destination_point_name)

  -- hidden Gibdo and chest
  sol.map.enemy_set_group_enabled("hidden_enemy", false)
  if not sol.game.savegame_get_boolean(800) then
    sol.map.chest_set_enabled("hidden_enemy_chest", false)
  end
end

function event_update()

  -- hidden enemies
  local x, y = sol.map.hero_get_position()
  if x > 1056 and x < 1200 and y > 1888 and y < 1968
      and not sol.map.chest_is_enabled("hidden_enemy_chest")
      and not sol.map.enemy_is_group_dead("hidden_enemy")
      and not sol.map.enemy_is_enabled("hidden_enemy_1") then

    sol.main.play_sound("cane")
    sol.map.enemy_set_enabled("hidden_enemy_1", true)
    sol.map.enemy_set_enabled("hidden_enemy_2", true)
  end
end

function event_enemy_dead(enemy_name)

  if enemy_name:find("^hidden_enemy")
      and sol.map.enemy_is_group_dead("hidden_enemy")
      and not sol.map.chest_is_enabled("hidden_enemy_chest") then
    sol.map.camera_move(1128, 2040, 250, function()
      sol.main.play_sound("chest_appears")
      sol.map.chest_set_enabled("hidden_enemy_chest", true)
    end)
  end
end


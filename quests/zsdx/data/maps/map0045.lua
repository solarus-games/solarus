-- Surprise wall

next_sign = 1
directions = {
  0, 1, 2, 1, 2, 1, 0, 3, 0, 1, 0, 3, 0, 1, 2, 3,
  0, 1, 2, 1, 0, 1, 2, 3, 2, 1, 0, 3, 0, 1, 0
}

function event_map_started(destination_point_name)
  
  if sol.game.savegame_get_boolean(139) then
    sol.map.tile_set_enabled("secret_way", false)
  end
end

function event_hero_interaction(entity_name)

  if string.find(entity_name, "^sign_") then
    if entity_name == "sign_"..next_sign then

      if next_sign < 32 then
        sol.map.dialog_start("surprise_wall.direction_"..directions[next_sign])
      elseif next_sign == 32 then
        sol.main.play_sound("secret")
	sol.map.tile_set_enabled("secret_way", false)
	sol.game.savegame_set_boolean(139, true)
      end
      next_sign = next_sign + 1
    else
      sol.main.play_sound("wrong")
      next_sign = 1
    end
  end
end


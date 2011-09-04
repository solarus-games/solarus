-- Waterfall cave 1F

nb_chests_open = 0
rupee_reward_variants = {
  1, 1, 2, 2, 3, 4
}

function event_map_started(destination_point_name)

  if sol.game.savegame_get_boolean(197) then
    -- piece of heart found
    for i = 1,7 do
      sol.map.chest_set_open("chest_"..i, true)
    end
  end
end

function event_chest_empty(chest_name)

  nb_chests_open = nb_chests_open + 1

  if nb_chests_open == 7 then
    sol.map.treasure_give("piece_of_heart", 1, 197)
  else
    sol.map.treasure_give("rupee", rupee_reward_variants[nb_chests_open], -1)
  end
end


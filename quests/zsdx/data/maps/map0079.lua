local map = ...
-- Waterfall cave 1F

nb_chests_open = 0
rupee_reward_variants = {
  1, 1, 2, 2, 3, 4
}

function map:on_started(destination_point_name)

  if map:get_game():get_boolean(197) then
    -- piece of heart found
    for i = 1, 7 do
      map:chest_set_open("chest_"..i, true)
    end
  end
end

function map:on_chest_empty(chest_name)

  nb_chests_open = nb_chests_open + 1

  if nb_chests_open == 7 then
    map:treasure_give("piece_of_heart", 1, 197)
  else
    map:treasure_give("rupee", rupee_reward_variants[nb_chests_open], -1)
  end
end


-- Chests game cave

playing = false
chest_open = ""
rewards = {
  {item_name = "wooden_key", variant = 1, savegame_variable = 180},
  {item_name = "wooden_key", variant = 1, savegame_variable = 180},
  {item_name = "wooden_key", variant = 1, savegame_variable = 180},
  {item_name = "wooden_key", variant = 1, savegame_variable = 180},
  {item_name = "wooden_key", variant = 1, savegame_variable = 180},
  {item_name = "piece_of_heart", variant = 1, savegame_variable = 181},
  {item_name = "piece_of_heart", variant = 1, savegame_variable = 181},
  {item_name = "piece_of_heart", variant = 1, savegame_variable = 181},
  {item_name = "heart", variant = 1, savegame_variable = -1},
  {item_name = "rupee", variant = 1, savegame_variable = -1},
  {item_name = "rupee", variant = 3, savegame_variable = -1},
  {item_name = "rupee", variant = 4, savegame_variable = -1},
  {item_name = "bomb", variant = 3, savegame_variable = -1},
  {item_name = "arrow", variant = 3, savegame_variable = -1},
  {item_name = "magic_flask", variant = 2, savegame_variable = -1},
  {item_name = "croissant", variant = 1, savegame_variable = -1}
}

function event_map_started(destination_point_name)

  if not sol.game.is_dungeon_finished(6) then
    sol.map.npc_remove("mini_game_npc")
  end
end

function event_npc_dialog(npc_name)

  if playing then
    sol.map.dialog_start("chests_game_cave.already_playing")
  elseif not sol.game.savegame_get_boolean(160) then
    -- first time
    sol.map.dialog_start("chests_game_cave.first_time")
    sol.game.savegame_set_boolean(160, true)
  else
    sol.map.dialog_start("chests_game_cave.not_first_time")
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "chests_game_cave.first_time"
      or first_message_id == "chests_game_cave.not_first_time" then

    if answer == 0 then
      if sol.game.get_money() >= 30 then
        sol.game.remove_money(30)
        playing = true

        if chest_open ~= "" then
          sol.map.chest_set_open(chest_open, false)
        end

        if not sol.game.savegame_get_boolean(180) then
          sol.map.dialog_start("chests_game_cave.start_game_wooden_key")
        elseif not sol.game.savegame_get_boolean(181) then
          sol.map.dialog_start("chests_game_cave.start_game_piece_of_heart")
        else
          sol.map.dialog_start("chests_game_cave.start_game")
        end
      else
        sol.main.play_sound("wrong")
        sol.map.dialog_start("chests_game_cave.not_enough_money")
      end
    end
  end

end

function event_chest_empty(chest_name)

  sol.map.hero_unfreeze()
  if playing then

    chest_open = chest_name

    -- choose a random treasure
    index = math.random(#rewards)

    while sol.game.savegame_get_boolean(rewards[index].savegame_variable) do
      -- don't give the wooden key or the piece of heart twice
      index = math.random(#rewards)
    end

    sol.map.treasure_give(rewards[index].item_name, rewards[index].variant, rewards[index].savegame_variable)
    playing = false
  else
    sol.main.play_sound("wrong")
    sol.map.chest_set_open(chest_name, false)
  end
end


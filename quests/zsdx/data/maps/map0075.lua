local map = ...
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

function map:on_started(destination_point)

  if not map:get_game():is_dungeon_finished(6) then
    mini_game_npc:remove()
  end
end

function map:on_npc_interaction(npc_name)

  if playing then
    map:start_dialog("chests_game_cave.already_playing")
  elseif not map:get_game():get_boolean(160) then
    -- first time
    map:start_dialog("chests_game_cave.first_time")
    map:get_game():set_boolean(160, true)
  else
    map:start_dialog("chests_game_cave.not_first_time")
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "chests_game_cave.first_time"
      or dialog_id == "chests_game_cave.not_first_time" then

    if answer == 0 then
      if map:get_game():get_money() >= 30 then
        map:get_game():remove_money(30)
        playing = true

        if chest_open ~= "" then
          map:get_entity(chest_open):set_open(false)
        end

        if not map:get_game():get_boolean(180) then
          map:start_dialog("chests_game_cave.start_game_wooden_key")
        elseif not map:get_game():get_boolean(181) then
          map:start_dialog("chests_game_cave.start_game_piece_of_heart")
        else
          map:start_dialog("chests_game_cave.start_game")
        end
      else
        sol.audio.play_sound("wrong")
        map:start_dialog("chests_game_cave.not_enough_money")
      end
    end
  end

end

function map:on_chest_empty(chest_name)

  map:get_hero():unfreeze()
  if playing then

    chest_open = chest_name

    -- choose a random treasure
    local index = math.random(#rewards)

    while rewards[index].savegame_variable ~= -1 and
        map:get_game():get_boolean(rewards[index].savegame_variable) do
      -- don't give a saved reward twice (wooden key or piece of heart)
      index = math.random(#rewards)
    end

    map:get_hero():start_treasure(rewards[index].item_name, rewards[index].variant, rewards[index].savegame_variable)
    playing = false
  else
    sol.audio.play_sound("wrong")
    map:get_entity(chest_name):set_open(false)
  end
end


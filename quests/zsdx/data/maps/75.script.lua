local map = ...
-- Chests game cave

local playing = false
local chest_open = nil
local rewards = {
  {item_name = "wooden_key", variant = 1, savegame_variable = "b180"},
  {item_name = "wooden_key", variant = 1, savegame_variable = "b180"},
  {item_name = "wooden_key", variant = 1, savegame_variable = "b180"},
  {item_name = "wooden_key", variant = 1, savegame_variable = "b180"},
  {item_name = "wooden_key", variant = 1, savegame_variable = "b180"},
  {item_name = "piece_of_heart", variant = 1, savegame_variable = "b181"},
  {item_name = "piece_of_heart", variant = 1, savegame_variable = "b181"},
  {item_name = "piece_of_heart", variant = 1, savegame_variable = "b181"},
  {item_name = "heart", variant = 1},
  {item_name = "rupee", variant = 1},
  {item_name = "rupee", variant = 3},
  {item_name = "rupee", variant = 4},
  {item_name = "bomb", variant = 3},
  {item_name = "arrow", variant = 3},
  {item_name = "magic_flask", variant = 2},
  {item_name = "croissant", variant = 1},
}

function map:on_started(destination)

  if not map:get_game():is_dungeon_finished(6) then
    mini_game_npc:remove()
  end
end

local function play_question_dialog_finished(answer)

  if answer == 0 then
    if map:get_game():get_money() >= 30 then
      map:get_game():remove_money(30)
      playing = true

      if chest_open ~= nil then
        chest_open:set_open(false)
      end

      if not map:get_game():get_value("b180") then
        map:start_dialog("chests_game_cave.start_game_wooden_key")
      elseif not map:get_game():get_value("b181") then
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

function mini_game_npc:on_interaction()

  if playing then
    map:start_dialog("chests_game_cave.already_playing")
  elseif not map:get_game():get_value("b160") then
    -- first time
    map:start_dialog("chests_game_cave.first_time", play_question_dialog_finished)
    map:get_game():set_value("b160", true)
  else
    map:start_dialog("chests_game_cave.not_first_time", play_question_dialog_finished)
  end
end

local function chest_empty(chest)

  hero:unfreeze()
  if playing then

    chest_open = chest

    -- choose a random treasure
    local index = math.random(#rewards)

    while rewards[index].savegame_variable ~= nil and
        map:get_game():get_value("b" .. rewards[index].savegame_variable) do
      -- don't give a saved reward twice (wooden key or piece of heart)
      index = math.random(#rewards)
    end

    hero:start_treasure(rewards[index].item_name, rewards[index].variant, rewards[index].savegame_variable)
    playing = false
  else
    sol.audio.play_sound("wrong")
    chest:set_open(false)
  end
end
for _, chest in ipairs(map:get_entities("chest_")) do
  chest:on_empty = chest_empty
end


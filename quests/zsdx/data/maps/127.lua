local map = ...
-- Hidden palace end

function map:on_started(destination_point)

  map:set_light(0)
end

function sword_chest:on_empty()

  local variant = 2
  if map:get_game():get_ability("sword") >= 2 then
    -- already got sword 2
    variant = 3
  end
  hero:start_treasure("sword", variant, -1)
end

function hero:on_obtaining_treasure(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.audio.play_music("excalibur")
  end
end

function hero:on_obtained_treasure(item_name, variant, savegame_variable)

  if item_name == "sword" then
    hero:start_victory()
  end
end

function hero:on_victory_finished()

  hero:unfreeze()
  sol.audio.play_music("light_world_dungeon")
end


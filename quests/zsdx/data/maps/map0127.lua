local map = ...
-- Hidden palace end

function map:on_started(destination_point_name)

  map:set_light(0)
end

function map:on_chest_empty(chest_name)

  if chest_name == "sword_chest" then
    local variant = 2
    if map:get_game():get_ability("sword") >= 2 then
      -- already got sword 2
      variant = 3
    end
    map:hero_start_treasure("sword", variant, -1)
  end
end

function map:on_treasure_obtaining(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.audio.play_music("excalibur")
  end
end

function map:on_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "sword" then
    map:hero_start_victory_sequence()
  end
end

function map:on_hero_victory_sequence_finished()

  map:hero_unfreeze()
  sol.audio.play_music("light_world_dungeon")
end


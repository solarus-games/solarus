-- Hidden palace end

function event_map_started(destination_point_name)

  sol.map.light_set(0)
end

function event_chest_empty(chest_name)

  if chest_name == "sword_chest" then
    local variant = 2
    if sol.game.get_ability("sword") >= 2 then
      -- already got sword 2
      variant = 3
    end
    sol.map.treasure_give("sword", variant, -1)
  end
end

function event_treasure_obtaining(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.main.play_music("excalibur.spc")
  end
end

function event_treasure_obtained(item_name, variant, savegame_variable)

  if item_name == "sword" then
    sol.map.hero_start_victory_sequence()
  end
end

function event_hero_victory_sequence_finished()

  sol.map.hero_unfreeze()
  sol.main.play_music("light_world_dungeon.spc")
end


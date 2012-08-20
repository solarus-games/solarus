local map = ...
-- Dungeon 4 icy room

function map:on_map_opening_transition_finished(destination_point_name)

  if map:get_game():get_item("tunic") < 2 then

    map:start_dialog("dungeon_4.too_cold")
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "dungeon_4.too_cold" then
    map:hero_walk("2222", false, true)
  end
end


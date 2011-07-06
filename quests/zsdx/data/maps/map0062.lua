-- Dungeon 4 icy room

function event_map_opening_transition_finished(destination_point_name)

  if sol.game.get_item("tunic") < 2 then

    sol.map.dialog_start("dungeon_4.too_cold")
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "dungeon_4.too_cold" then
    sol.map.hero_walk("2222", false, true)
  end
end


function event_npc_interaction(npc_name)

  if not sol.game.savegame_get_boolean(61) then
    sol.map.dialog_start("water_house.give_bottle")
  else
    sol.map.dialog_start("water_house.after")
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "water_house.give_bottle" then
    sol.map.treasure_give("bottle_2", 2, 61)
  end
end


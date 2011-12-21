-- Desert east house

function event_npc_interaction(npc_name)

  if sol.game.savegame_get_boolean(59) then
    -- door already open
    sol.map.dialog_start("desert.east_house.bone_key")
  elseif sol.game.savegame_get_boolean(60) then
    -- died in the cursed cave
    sol.map.dialog_start("desert.east_house.back_from_dead")
  else
    sol.map.dialog_start("desert.east_house.first_time")
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "desert.east_house.back_from_dead" then
    sol.map.door_open("door")
  end
end


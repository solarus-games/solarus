-- Hidden palace D1

function event_map_started(destination_point_name)

  sol.map.light_set(0)

  if sol.game.savegame_get_boolean(934) then
    sol.map.npc_remove("bone_key_door")
  end
end

-- Function called when the player presses the action key on the door
function event_npc_interaction(npc_name)

  if npc_name == "bone_key_door" then
    if not sol.game.has_item("bone_key") then
      sol.map.dialog_start("hidden_palace.door_closed")
    else
      sol.map.dialog_start("hidden_palace.using_bone_key")
    end
  end
end

function event_dialog_finished(first_message_id)

  if first_message_id == "hidden_palace.using_bone_key" then
    sol.main.play_sound("door_open")
    sol.main.play_sound("door_unlocked")
    sol.main.play_sound("secret")
    sol.map.npc_remove("bone_key_door")
    sol.game.savegame_set_boolean(934, true)
  end
end


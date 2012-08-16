local map = ...
-- Hidden palace D1

function map:on_started(destination_point_name)

  map:light_set(0)

  if map:get_game():get_boolean(934) then
    map:npc_remove("bone_key_door")
  end
end

-- Function called when the player presses the action key on the door
function map:on_npc_interaction(npc_name)

  if npc_name == "bone_key_door" then
    if not map:get_game():has_item("bone_key") then
      map:dialog_start("hidden_palace.door_closed")
    else
      map:dialog_start("hidden_palace.using_bone_key")
    end
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "hidden_palace.using_bone_key" then
    sol.audio.play_sound("door_open")
    sol.audio.play_sound("door_unlocked")
    sol.audio.play_sound("secret")
    map:npc_remove("bone_key_door")
    map:get_game():set_boolean(934, true)
  end
end


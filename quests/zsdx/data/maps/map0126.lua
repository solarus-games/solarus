local map = ...
-- Hidden palace D1

function map:on_started(destination_point)

  map:set_light(0)

  if map:get_game():get_boolean(934) then
    bone_key_door:remove()
  end
end

-- Function called when the player presses the action key on the door
function map:on_npc_interaction(npc_name)

  if npc_name == "bone_key_door" then
    if not map:get_game():get_item("bone_key"):has_variant() then
      map:start_dialog("hidden_palace.door_closed")
    else
      map:start_dialog("hidden_palace.using_bone_key")
    end
  end
end

function map:on_dialog_finished(dialog_id)

  if dialog_id == "hidden_palace.using_bone_key" then
    sol.audio.play_sound("door_open")
    sol.audio.play_sound("door_unlocked")
    sol.audio.play_sound("secret")
    bone_key_door:remove()
    map:get_game():set_boolean(934, true)
  end
end


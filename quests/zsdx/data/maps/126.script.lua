local map = ...
-- Hidden palace D1

function map:on_started(destination)

  map:set_light(0)

  if map:get_game():get_value("b934") then
    bone_key_door:remove()
  end
end

-- Function called when the player presses the action key on the door
function bone_key_door:on_interaction()

  if not map:get_game():get_item("bone_key"):has_variant() then
    map:start_dialog("hidden_palace.door_closed")
  else
    map:start_dialog("hidden_palace.using_bone_key", function()
      sol.audio.play_sound("door_open")
      sol.audio.play_sound("door_unlocked")
      sol.audio.play_sound("secret")
      bone_key_door:remove()
      map:get_game():set_value("b934", true)
    end)
  end
end


------------------------------
-- Outside fields SO script --
------------------------------

-- Function called when the player wants to talk to a non-playing character.
-- If the NPC is the guard, then the dialog starts.
function event_npc_dialog(npc_name)
  if string.find(npc_name, "guard") then
    if sol.map.hero_get_direction() == 0 then
      sol.map.dialog_start("outside_fields_SO.guard_ok")
    else
      sol.map.dialog_start("outside_fields_SO.guard_nok")
    end
  end
end

-- Function called when the dialog box is being closed.
-- If the player was talking to the guard, we do the appropriate action
function event_dialog_finished(first_message_id, answer)

   if first_message_id == "outside_fields_SO.guard_ok" then
      -- make the guard move
      m = sol.main.path_movement_create("000000000066", 24)
      s = sol.map.npc_get_sprite("guard")
      sol.main.sprite_set_animation(s, "walking")
      sol.map.npc_start_movement("guard", m)
  end
end

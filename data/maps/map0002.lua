finished = false

function event_npc_dialog(npc_name)

   if (npc_name == "game_1_npc") then
      npc_walk(npc_name, "444", false)
      freeze();
   end
end

function event_npc_path_finished(npc_name)

   if (npc_name == "game_1_npc") then

      if (not finished) then
	 npc_walk(npc_name, "000", false)
	 finished = true
      else
	 npc_set_direction(npc_name, 3)
	 unfreeze();
      end
   end
end

function event_open_chest(chest_name)
   start_message("msg");
end

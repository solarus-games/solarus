function event_map_started()

   if savegame_get_boolean(24) then
      npc_remove("monkey")
   end

   npc_random_walk("hat_man")
   npc_random_walk("grand_son")
end

--------------------------
-- Grandma house script --
--------------------------

-- The player talks to grandma
function event_npc_dialog(npc_name)

   if not savegame_get_boolean(30) then
      -- beginning: go get a sword
      start_message("grandma_house.sword")
   else
      -- with the sword: find Sahasrahla
      start_message("grandma_house.find_sahasrahla")
   end

   -- TODO other messages later in the game
end

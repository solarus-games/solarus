-----------------------------
-- Sahasrahla house script --
-----------------------------

-- The player talks to Sahasrahla
function event_npc_dialog(npc_name)

   if not savegame_get_boolean(33) then
      -- first visit
      start_message("sahasrahla_house.beginning")
      set_message_variable("sahasrahla_house.beginning", get_player_name())
      set_message_variable("sahasrahla_house.beginning", get_player_name())

   elseif not savegame_get_boolean(34) then
      -- the player already came and accepted the mission
      start_message("sahasrahla_house.quest_accepted")
	 
   elseif not savegame_get_boolean(35) then
      -- the player has seen the frozen door but was not able to unfreeze it
      start_message("sahasrahla_house.frozen_door_advice")

   else 
      start_message("sahasrahla_house.door_unfreezed")
      -- TODO other messages later in the game
   end
end

-- Function called when the dialog box is being closed
-- first_message_id: name of the first message of the sequence that has just finished
-- answer: the answer of the question (0 or 1) or -1 if there was no question
function event_message_sequence_finished(first_message_id, answer)

   if first_message_id == "sahasrahla_house.beginning" then
      -- give the world map to the player
      give_treasure(50, 33)
   end
end

-- Function called when the player has just obtained a treasure
function event_obtained_treasure(content, savegame_variable)
   start_message("sahasrahla_house.quest_accepted")
end

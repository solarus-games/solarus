-----------------------
-- Smith cave script --
-----------------------

-- Function called when the player wants to talk to a non-playing character.
function event_npc_dialog(npc_name)

   -- smith dialog
   if equipment_get_sword() == 0 then
      -- the player has no sword yet
      start_message("smith_cave.without_sword")
   else
      -- the player already has the sword
      start_message("smith_cave.with_sword")
   end
end

-- Function called when the dialog box is being closed.
function event_message_sequence_finished(first_message_id, answer)

   if first_message_id == "smith_cave.without_sword" then
      -- the dialog was the question to buy the sword

      if answer == 1 then
	 -- the player does not want to buy the sword
	 start_message("smith_cave.not_buying")
      else
	 -- wants to buy the sword
	 if get_rupees() < 80 then
	    -- not enough money
	    play_sound("wrong")
	    start_message("smith_cave.not_enough_money")
	 else
	    -- enough money: buy the sword
	    remove_rupees(80)
	    give_treasure(66, 30)
	 end
      end
   end
end

-- Function called when the player has bought the sword.
function event_obtained_treasure(content, savegame_variable)
   if (content == 66) then
      start_message("smith_cave.thank_you")
   end
end

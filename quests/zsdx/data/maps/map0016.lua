-----------------------
-- Smith cave script --
-----------------------

-- Function called when the player wants to talk to a non-playing character.
function event_npc_dialog(npc_name)

   -- smith dialog
   if not savegame_get_boolean(30) then
      -- the player has no sword yet
      dialog_start("smith_cave.without_sword")
   else
      -- the player already has the sword
      dialog_start("smith_cave.with_sword")
   end
end

-- Function called when the dialog box is being closed.
function event_dialog_finished(first_message_id, answer)

   if first_message_id == "smith_cave.without_sword" then
      -- the dialog was the question to buy the sword

      if answer == 1 then
	 -- the player does not want to buy the sword
	 dialog_start("smith_cave.not_buying")
      else
	 -- wants to buy the sword
	 if equipment_get_rupees() < 80 then
	    -- not enough money
	    play_sound("wrong")
	    dialog_start("smith_cave.not_enough_money")
	 else
	    -- enough money: buy the sword
	    equipment_remove_rupees(80)
	    treasure_give(66, 30)
	 end
      end
   end
end

-- Function called when the player has bought the sword.
function event_treasure_obtained(content, savegame_variable)
   if (content == 66) then
      dialog_start("smith_cave.thank_you")
   end
end

math.randomseed(os.time())
playing_game_1 = false
already_played_game_1 = false
rewards = {5, 20, 50}

-- Dialog with the rupee house NPCs.
function event_npc_dialog(npc_name)

   if npc_name == "game_1_npc" then

      -- game 1 dialog
      if not playing_game_1 then

	 -- see if the player can still play
	 allowed_to_play = not get_savegame_boolean(16)

	 if allowed_to_play then
	    if not already_played_game_1 then
	       -- long dialog with game rules
	       start_message("rupee_house.game_1.intro")
	    else
	       -- quick dialog to play again
	       start_message("rupee_house.game_1.play_again_question")
	    end
	 else
	    -- the player already won much money
	    start_message("rupee_house.game_1.not_allowed_to_play")
	 end
      else
	 -- the player is already playing: tell him to choose a chest
	 start_message("rupee_house.game_1.choose_chest")
      end
   end
end

-- Opening a chest in game 1
function event_open_empty_chest(chest_name)

   if not playing_game_1 then
      -- trying to open a chest but not playing yet!
      start_message("rupee_house.game_1.pay_first") -- the game 1 man is angry
      set_chest_open(chest_name, false) -- close the chest again
      play_sound("wrong")
      unfreeze() -- restore the control
   else
      -- give a random reward
      index = math.random(#rewards)
      amount = rewards[index]
      give_treasure_with_amount(87, amount, -1)

      if amount == 50 then
	 -- maximum reward found: game will now refuse to let the hero play again
	 set_savegame_boolean(16, true)
      end

      playing_game_1 = false
      already_played_game_1 = true
   end
end

-- Getting an answer from a dialog box
function event_message_sequence_finished(first_message_id, answer)

   if first_message_id == "rupee_house.game_1.intro" or 
      first_message_id == "rupee_house.game_1.play_again_question" then

      if answer == "0" then
	 -- wants to play game 1

	 if get_rupees() >= 20 then
	    -- enough money

	    -- reset the 3 chests
	    set_chest_open("chest_1", false)
	    set_chest_open("chest_2", false)
	    set_chest_open("chest_3", false)

	    remove_rupees(20)
	    start_message("rupee_house.game_1.good_luck")
	    playing_game_1 = true
	 else
	     -- not enough money
	    play_sound("wrong")
	    start_message("rupee_house.not_enough_money")
	 end
      else
	 -- don't want to play game 1
	 start_message("rupee_house.not_playing")
      end
   end

end

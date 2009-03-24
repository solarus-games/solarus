-- Rupee house script

-- Initializations made when the map has just been loaded
playing_game_1 = false
playing_game_3 = false
already_played_game_1 = false
rewards = {5, 20, 50} -- possible rupee rewards in game 1

-- game 3 behavior
slot_machine_delays = {slot_machine_left = 100, slot_machine_middle = 50, slot_machine_right = 200};

-- Function called when the map starts
function event_map_started()
   interactive_entity_set_animation_frame("slot_machine_left", 6)
   interactive_entity_set_animation_frame("slot_machine_middle", 15)
   interactive_entity_set_animation_frame("slot_machine_right", 9)
end

-- Function called when the player wants to talk to a non-playing character
-- npc_name: name of the NPC the player is talking to
function event_npc_dialog(npc_name)

   if npc_name == "game_1_npc" then
      -- game 1 dialog

      if playing_game_1 then
	 -- the player is already playing: tell him to choose a chest
	 start_message("rupee_house.game_1.choose_chest")
      else

	 -- see if the player can still play
	 unauthorized = get_savegame_boolean(16)

	 if unauthorized then
	    -- the player already won much money
	    start_message("rupee_house.game_1.not_allowed_to_play")
	 else 
	    if not already_played_game_1 then
	       -- first time: long dialog with game rules
	       start_message("rupee_house.game_1.intro")
	    else
	       -- quick dialog to play again
	       start_message("rupee_house.game_1.play_again_question")
	    end
	 end
      end

   elseif npc_name == "game_3_npc" then
      -- game 3 dialog

      if playing_game_3 then
	 -- the player is already playing: let him restart the game
	 start_message("rupee_house.game_3.restart_question")
      else
	 -- see if the player can still play
	 unauthorized = get_savegame_boolean(17)

	 if unauthorized then
	    -- the player already won this game
	    start_message("rupee_house.game_3.not_allowed_to_play")
	 else
	    -- game rules
	    start_message("rupee_house.game_3.intro")
	 end
      end
   end
end

-- Function called when the dialog box is being closed
-- first_message_id: name of the first message of the sequence that has just finished
-- answer: the answer of the question (0 or 1) or -1 if there was no question
function event_message_sequence_finished(first_message_id, answer)

   -- if the dialog was the game 1 question
   if first_message_id == "rupee_house.game_1.intro" or 
      first_message_id == "rupee_house.game_1.play_again_question" then

      if answer == "1" then
	 -- don't want to play the game
	 start_message("rupee_house.game_1.not_playing")
      else
	 -- wants to play game 1

	 if get_rupees() < 20 then
	     -- not enough money
	    play_sound("wrong")
	    start_message("rupee_house.not_enough_money")

	 else
	    -- enough money: reset the 3 chests, pay and start the game
	    set_chest_open("chest_1", false)
	    set_chest_open("chest_2", false)
	    set_chest_open("chest_3", false)

	    remove_rupees(20)
	    start_message("rupee_house.game_1.good_luck")
	    playing_game_1 = true
	 end
      end

   elseif first_message_id == "rupee_house.game_3.intro" or 
      first_message_id == "rupee_house.game_3.restart_question" then
      -- if the dialog was the game 3 question

      if answer == "1" then
	 -- don't want to play the game
	 start_message("rupee_house.game_3.not_playing")
      else
	 -- wants to play game 3

	 if get_rupees() < 10 then
	     -- not enough money
	    play_sound("wrong")
	    start_message("rupee_house.not_enough_money")

	 else
	    -- enough money: reset the game, pay and start the game

	    reset_blocks();
	    disable_tile("game_3_barrier_1");
	    disable_tile("game_3_barrier_2");
	    disable_tile("game_3_barrier_3");
	    disable_tile("game_3_middle_barrier");
	    stop_timer("game_3_timer")

	    remove_rupees(10)
	    start_message("rupee_house.game_3.go")
	    start_timer(6000, "game_3_timer", true);
	    playing_game_3 = true
	 end
      end
   end
end

-- Function called when the player opens an empty chest (i.e. a chest
-- whose feature is to call the script).
-- chest_name: name of the chest being open
function event_open_empty_chest(chest_name)

   if not playing_game_1 then
      -- trying to open a chest but not playing yet
      start_message("rupee_house.game_1.pay_first") -- the game man is angry
      set_chest_open(chest_name, false) -- close the chest again
      play_sound("wrong")
      unfreeze() -- restore the control
   else
      -- give a random reward
      index = math.random(#rewards)
      amount = rewards[index]

      -- 87 means green rupees (see include/Treasure.h), amount is the number of rupees to give
      -- and -1 means that the chest is not saved
      give_treasure_with_amount(87, amount, -1)

      if amount == 50 then
	 -- the maximum reward was found: the game will now refuse to let the hero play again
	 set_savegame_boolean(16, true)
      end

      playing_game_1 = false
      already_played_game_1 = true
   end
end

-- Function called when the timer of game 3 ends.
function game_3_timer()
   play_sound("door_closed")
   enable_tile("game_3_middle_barrier")
end

-- Function called when the player obtains a treasure
function event_got_treasure(content, savegame_variable)

   -- stop game 3 when the player founds the piece of heart
   if savegame_variable == "17" then
      disable_tile("game_3_final_barrier")
      playing_game_3 = false
   end
end

-- Function called when a switch is enabled
function event_switch_enabled(switch_name)

   -- stop the timer when the player reaches the invisible switch
   stop_timer("game_3_timer")
   play_sound("secret")
end

function event_interaction(entity_name)

   -- start the animation of this entity
   interactive_entity_set_animation(entity_name, "started")
   interactive_entity_set_animation_delay(entity_name, slot_machine_delays[entity_name])
end

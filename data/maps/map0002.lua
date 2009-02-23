math.randomseed(os.time())
playing_game_1 = false
rewards = {0, 10, 20, 20, 50, 100}

function event_npc_dialog(npc_name)

   if (npc_name == "game_1_npc") then
      if (not playing_game_1) then
	 start_message("rupee_house.game_1.intro")
      else
	 start_message("rupee_house.game_1.choose_chest")
      end
   end
end

function event_open_chest(chest_name)

   if (not playing_game_1) then
      start_message("rupee_house.game_1.pay_first")
      set_chest_open(chest_name, false)
      play_sound("wrong")
   else
      index = math.random(#rewards)
      give_treasure_with_amount(87, rewards[index], -1)
   end
end

function event_message_sequence_finished(first_message_id, answer)

   if (first_message_id == "rupee_house.game_1.intro") then

      if (answer == 1) then

	 if (get_rupees() >= 20) then
	    remove_rupees(20)
	    start_message("rupee_house.game_1.good_luck")
	    playing_game_1 = true
	 else
	    start_message("rupee_house.not_enough_money")
	 end
      else
	 start_message("rupee_house.not_playing")
      end
   end

end

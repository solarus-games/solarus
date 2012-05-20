-- Broken rupee house

function event_npc_interaction(npc_name)

  if not sol.map.get_game():get_boolean(156) then

    if npc_name == "game_1_man" then
      sol.map.dialog_start("rupee_house_broken.help_us_left")
    elseif npc_name == "game_2_man" then
      sol.map.dialog_start("rupee_house_broken.help_us_middle")
    elseif npc_name == "game_3_man" then
      sol.map.dialog_start("rupee_house_broken.help_us_right")
    end
  elseif not sol.map.get_game():get_boolean(157) then
    sol.map.dialog_start("rupee_house_broken.thanks")
  else
    sol.map.dialog_start("rupee_house_broken.thanks_again")
  end
end

function event_dialog_finished(dialog_id, answer)

  if dialog_id == "rupee_house_broken.thanks" then
    sol.map.treasure_give("flippers", 1, 157)
  end
end


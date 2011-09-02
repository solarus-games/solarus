-- Broken rupee house

function event_npc_dialog(npc_name)

  if not sol.game.savegame_get_boolean(156) then

    if npc_name == "game_1_man" then
      sol.map.dialog_start("rupee_house_broken.help_us_left")
    elseif npc_name == "game_2_man" then
      sol.map.dialog_start("rupee_house_broken.help_us_middle")
    elseif npc_name == "game_3_man" then
      sol.map.dialog_start("rupee_house_broken.help_us_right")
    end
  elseif not sol.game.savegame_get_boolean(157) then
    sol.map.dialog_start("rupee_house_broken.thanks")
  else
    sol.map.dialog_start("rupee_house_broken.thanks_again")
  end
end

function event_dialog_finished(first_message_id, answer)

  if first_message_id == "rupee_house_broken.thanks" then
    sol.map.treasure_give("flippers", 1, 157)
  end
end


local map = ...
-- Broken rupee house

function map:on_npc_interaction(npc_name)

  if not map:get_game():get_boolean(156) then

    if npc_name == "game_1_man" then
      map:start_dialog("rupee_house_broken.help_us_left")
    elseif npc_name == "game_2_man" then
      map:start_dialog("rupee_house_broken.help_us_middle")
    elseif npc_name == "game_3_man" then
      map:start_dialog("rupee_house_broken.help_us_right")
    end
  elseif not map:get_game():get_boolean(157) then
    map:start_dialog("rupee_house_broken.thanks")
  else
    map:start_dialog("rupee_house_broken.thanks_again")
  end
end

function map:on_dialog_finished(dialog_id, answer)

  if dialog_id == "rupee_house_broken.thanks" then
    hero:start_treasure("flippers", 1, 157)
  end
end


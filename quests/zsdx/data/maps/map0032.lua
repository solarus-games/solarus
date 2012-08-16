local map = ...
-- Telepathic booth

function map:on_npc_interaction(npc_name)

  if npc_name == "hint_stone" then

    if not map:get_game():is_dungeon_finished(1) then
      map:dialog_start("telepathic_booth.not_working")
    elseif not map:get_game():has_item("bow") then
      map:dialog_start("telepathic_booth.go_sahasrahla")
      map:dialog_set_variable("telepathic_booth.go_sahasrahla", map:get_game():get_player_name())
    elseif not map:get_game():is_dungeon_finished(2) then
      map:dialog_start("telepathic_booth.go_twin_caves")
      map:dialog_set_variable("telepathic_booth.go_twin_caves", map:get_game():get_player_name())
    elseif map:get_game():get_item("rupee_bag") < 2 then
      map:dialog_start("telepathic_booth.dungeon_2_not_really_finished")
      map:dialog_set_variable("telepathic_booth.dungeon_2_not_really_finished", map:get_game():get_player_name())
    elseif not map:get_game():is_dungeon_finished(3) then
      map:dialog_start("telepathic_booth.go_master_arbror")
      map:dialog_set_variable("telepathic_booth.go_master_arbror", map:get_game():get_player_name())
    elseif not map:get_game():is_dungeon_finished(4) then
      map:dialog_start("telepathic_booth.go_billy")
      map:dialog_set_variable("telepathic_booth.go_billy", map:get_game():get_player_name())
    else
      map:dialog_start("telepathic_booth.shop")
      map:dialog_set_variable("telepathic_booth.shop", map:get_game():get_player_name())
    end
  end
end


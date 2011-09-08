----------------------
-- Telepathic booth --
----------------------

function event_hero_interaction(entity_name)

  if entity_name == "hint_stone" then

    if not sol.game.is_dungeon_finished(2) then
      sol.map.dialog_start("telepathic_booth.not_working")
    elseif not sol.game.is_dungeon_finished(3) then
      sol.map.dialog_start("telepathic_booth.go_master_arbror")
      sol.map.dialog_set_variable("telepathic_booth.go_master_arbror", sol.game.savegame_get_name())
    elseif not sol.game.is_dungeon_finished(4) then
      sol.map.dialog_start("telepathic_booth.go_billy")
      sol.map.dialog_set_variable("telepathic_booth.go_billy", sol.game.savegame_get_name())
    elseif not sol.game.is_dungeon_finished(5) then
      sol.map.dialog_start("telepathic_booth.hahaha")
      sol.map.dialog_set_variable("telepathic_booth.hahaha", sol.game.savegame_get_name())
    end
    -- TODO after dungeon 5
  end
end


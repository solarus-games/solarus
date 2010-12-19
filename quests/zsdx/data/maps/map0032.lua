----------------------
-- Telepathic booth --
----------------------

function event_hero_interaction(entity_name)

  if entity_name == "hint_stone" then

    if not sol.game.savegame_get_boolean(103) then
      sol.map.dialog_start("telepathic_booth.not_working")
    else
      sol.map.dialog_start("telepathic_booth.go_master_arbror")
      sol.map.dialog_set_variable("telepathic_booth.go_master_arbror", sol.game.savegame_get_name())
    end
    -- TODO after dungeon 3
  end
end


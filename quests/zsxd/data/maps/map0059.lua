-- Caverne maudite -16

function event_map_started()
  -- before dying, save the fact that the cursed cave vas visited
  sol.game.savegame_set_boolean(60, 1)
end

function event_update()

  -- make sure the hero will die in the lava
  -- even if a fairy saves him
  if sol.game.get_life() > 1 then
    sol.game.remove_life(sol.game.get_life() - 1)
  end
end



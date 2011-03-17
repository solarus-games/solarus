-- Caverne maudite -16

function event_update()

  -- make sure the hero will die in the lava
  -- even if a fairy saves him
  if sol.game.get_life() > 1 then
    sol.game.remove_life(sol.game.get_life() - 1)
  end
end



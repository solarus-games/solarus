function event_obtained(variant, savegame_variable)

  -- give the ability to see in the dark
  sol.game.set_ability("see_in_dark", 1)
  
  -- give the magic bar if necessary
  if sol.game.get_max_magic() == 0 then
    sol.game.set_max_magic(42)
  end
end

function event_use()

  magic_needed = 1
  if sol.game.get_magic() >= magic_needed then
    sol.main.play_sound("lamp")
    sol.game.remove_magic(magic_needed)
  else
    sol.main.play_sound("wrong")
  end
  sol.item.set_finished()
end


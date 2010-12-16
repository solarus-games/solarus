function event_obtained(variant, savegame_variable)

  if variant ~= 0 then

    sol.game.set_ability("see_in_dark", 1)
    if sol.game.get_max_magic() == 0 then
      sol.game.set_max_magic(42)
    end
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


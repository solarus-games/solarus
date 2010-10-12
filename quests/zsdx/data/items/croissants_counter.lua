function event_use()

  if sol.item.get_amount() == 0 then
    sol.main.play_sound("wrong")
  else
    sol.item.remove_amount(1)
    sol.game.equipment_add_life(4 * 7)
  end
  sol.item.set_finished()
end


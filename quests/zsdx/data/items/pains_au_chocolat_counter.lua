function event_use()

  if sol.item.get_amount() == 0 then
    sol.audio.play_sound("wrong")
  else
    sol.item.remove_amount(1)
    sol.map.get_game():add_life(4 * 3)
  end
  sol.item.set_finished()
end


function event_use()

  if sol.item.get_amount() == 0 then
    sol.main.play_sound("wrong")
  else
    sol.item.remove_amount(1)
    x, y, layer = sol.map.hero_get_position();
-- TODO   sol.map.bomb_create(x, y, layer)
  end
  sol.item.set_finished()
end


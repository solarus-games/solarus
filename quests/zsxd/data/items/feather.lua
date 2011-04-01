function event_use()
  sol.main.play_sound("jump")
  direction4 = sol.map.hero_get_direction()

  randdirection = math.random(1,4)
  diagonale = 0  

  if randdirection == 1 then
    diagonale = 1
  elseif randdirection == 4 then
    diagonale = -1
  end

  sol.map.hero_start_jumping((direction4 * 2 + diagonale ) % 8, math.random(16,100), false)
  sol.item.set_finished()
end


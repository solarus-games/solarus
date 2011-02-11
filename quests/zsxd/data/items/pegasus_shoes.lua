function event_use()
  sol.map.hero_set_direction( math.random(0,3) )
  --sol.map.hero_set_direction( (sol.map.hero_get_direction() + 2) % 4 )
  sol.map.hero_start_running()
  sol.item.set_finished()
end


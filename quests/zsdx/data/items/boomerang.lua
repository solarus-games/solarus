function event_use()

  if sol.item.get_variant() == 1 then
    sol.map.hero_start_boomerang(128, 160, "boomerang1", "entities/boomerang1")
  else
    -- boomerang 2: longer and faster movement
    sol.map.hero_start_boomerang(192, 320, "boomerang2", "entities/boomerang2")
  end
  sol.item.set_finished()
end


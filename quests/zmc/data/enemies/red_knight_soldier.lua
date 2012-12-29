local enemy = ...

-- Red knight soldier.

sol.main.load_file("enemies/generic_soldier")(enemy)
enemy:set_properties({
  main_sprite = "enemies/red_knight_soldier",
  sword_sprite = "enemies/red_knight_soldier_sword",
  life = 4,
  damage = 2,
  play_hero_seen_sound = true
})


local enemy = ...

-- Green knight soldier.

sol.main.load_file("enemies/generic_soldier")(enemy)
enemy:set_properties({
  main_sprite = "enemies/green_knight_soldier",
  sword_sprite = "enemies/green_knight_soldier_sword",
  life = 2,
  damage = 2,
  play_hero_seen_sound = true
})


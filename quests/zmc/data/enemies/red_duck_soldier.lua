local enemy = ...

-- Red duck soldier.

sol.main.load_file("enemies/generic_soldier")(enemy)
enemy:set_properties({
  main_sprite = "enemies/red_duck_soldier",
  sword_sprite = "enemies/red_duck_soldier_sword",
  life = 8,
  damage = 12,
  hurt_style = "monster",
  play_hero_seen_sound = true
})


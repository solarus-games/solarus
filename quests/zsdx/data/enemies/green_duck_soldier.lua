local enemy = ...

-- Green duck soldier.

sol.main.load_file("enemies/generic_soldier")(enemy)
enemy:set_properties({
  main_sprite = "enemies/green_duck_soldier",
  sword_sprite = "enemies/green_duck_soldier_sword",
  life = 4,
  damage = 8,
  hurt_style = "monster",
  play_hero_seen_sound = true
})


local enemy = ...

-- Bee Guard

sol.main.load_file("enemies/generic_soldier")(enemy)
enemy:set_properties({
  main_sprite = "enemies/bee_guard",
  sword_sprite = "enemies/bee_guard_sword",
  life = 3,
  damage = 2,
  play_hero_seen_sound = true,
  normal_speed = 32,
  faster_speed = 64,
  hurt_style = "monster"
})


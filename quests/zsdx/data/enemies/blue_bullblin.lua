local enemy = ...

-- Blue Bullblin

sol.main.do_file("enemies/generic_soldier")

set_properties({
  main_sprite = "enemies/blue_bullblin",
  sword_sprite = "enemies/blue_bullblin_sword",
  life = 3,
  damage = 2,
  play_hero_seen_sound = false,
  normal_speed = 32,
  faster_speed = 48,
})


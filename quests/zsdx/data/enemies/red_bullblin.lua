-- Red Bullblin

sol.main.do_file("enemies/generic_soldier")

set_properties({
  main_sprite = "enemies/red_bullblin",
  sword_sprite = "enemies/red_bullblin_sword",
  life = 4,
  damage = 4,
  play_hero_seen_sound = false,
  normal_speed = 32,
  faster_speed = 48,
})


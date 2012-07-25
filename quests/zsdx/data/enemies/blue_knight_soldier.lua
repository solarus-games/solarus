-- Blue knight soldier

sol.main.do_file("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/blue_knight_soldier",
  sword_sprite = "enemies/blue_knight_soldier_sword",
  life = 3,
  damage = 2,
  play_hero_seen_sound = true
})


-- Green knight soldier

sol.main.do_file("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/green_knight_soldier",
  sword_sprite = "enemies/green_knight_soldier_sword",
  life = 2,
  damage = 2,
  play_hero_seen_sound = true
})


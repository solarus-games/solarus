-- Bee Guard

sol.main.include("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/bee_guard",
  sword_sprite = "enemies/bee_guard_sword",
  life = 3,
  damage = 2,
  play_hero_seend_sound = true,
  normal_speed = 32,
  faster_speed = 64,
  hurt_sound_style = "monster"
})


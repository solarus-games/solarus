-- Lizalfos

sol.main.include("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/lizalfos",
  sword_sprite = "enemies/lizalfos_sword",
  life = 5,
  damage = 3,
  play_hero_seen_sound = true,
  hurt_sound_style = "monster",
  normal_speed = 48,
  faster_speed = 72
})


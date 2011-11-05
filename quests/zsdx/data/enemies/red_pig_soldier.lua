-- Red pig soldier

sol.main.include("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/red_pig_soldier",
  sword_sprite = "enemies/red_pig_soldier_sword",
  life = 12,
  damage = 6,
  play_hero_seen_sound = true,
  hurt_style = "monster"
})


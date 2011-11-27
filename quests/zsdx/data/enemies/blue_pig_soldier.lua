-- Blue pig soldier

sol.main.include("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/blue_pig_soldier",
  sword_sprite = "enemies/blue_pig_soldier_sword",
  life = 8,
  damage = 12,
  play_hero_seen_sound = true,
  hurt_style = "monster"
})


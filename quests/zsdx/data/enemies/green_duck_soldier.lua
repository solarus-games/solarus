-- Green duck soldier

sol.main.include("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/green_duck_soldier",
  sword_sprite = "enemies/green_duck_soldier_sword",
  life = 4,
  damage = 3,
  hurt_style = "monster",
  play_hero_seen_sound = true
})


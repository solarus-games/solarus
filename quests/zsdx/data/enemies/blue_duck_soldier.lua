-- Blue duck soldier

sol.main.include("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/blue_duck_soldier",
  sword_sprite = "enemies/blue_duck_soldier_sword",
  life = 5,
  damage = 10,
  hurt_style = "monster",
  play_hero_seen_sound = true
})


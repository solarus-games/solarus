-- Red duck soldier

sol.main.include("enemies/generic_soldier")
set_properties({
  main_sprite = "enemies/red_duck_soldier",
  sword_sprite = "enemies/red_duck_soldier_sword",
  life = 6,
  damage = 3,
  hurt_sound_style = "monster"
})


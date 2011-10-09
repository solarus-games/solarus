-- Ropa

sol.main.include("enemies/generic_towards_hero")
set_properties({
  sprite = "enemies/ropa",
  life = 6,
  damage = 4,
  normal_speed = 32,
  faster_speed = 32,
  hurt_sound_style = "monster",
  movement_create = sol.main.random_movement_create
})


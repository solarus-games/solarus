-- Billy

sol.main.include("enemies/generic_towards_hero")
set_properties({
  sprite = "enemies/billy",
  life = 30,
  damage = 12,
  normal_speed = 48,
  faster_speed = 72,
  hurt_style = "boss",
  push_hero_on_sword = true
})


-- Gibdo

sol.main.include("enemies/generic_towards_hero")
set_properties({
  sprite = "enemies/gibdo",
  life = 16,
  damage = 8,
  normal_speed = 40,
  faster_speed = 48,
  pushed_when_hurt = false
})


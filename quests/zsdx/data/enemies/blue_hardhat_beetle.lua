-- Blue Hardhat Beetle

sol.main.include("enemies/generic_towards_hero")
set_properties({
  sprite = "enemies/blue_hardhat_beetle",
  life = 5,
  damage = 4,
  normal_speed = 32,
  faster_speed = 48,
  hurt_style = "monster",
  push_hero_on_sword = true,
  movement_create = random_movement_create
})

function random_movement_create(speed)
  
  local m = sol.main.random_movement_create(speed)
  sol.main.movement_set_property(m, "smooth", true)
  return m
end


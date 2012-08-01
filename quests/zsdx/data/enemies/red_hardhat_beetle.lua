local enemy = ...

-- Red Hardhat Beetle

sol.main.do_file("enemies/generic_towards_hero")
set_properties({
  sprite = "enemies/red_hardhat_beetle",
  life = 8,
  damage = 6,
  normal_speed = 32,
  faster_speed = 48,
  hurt_style = "monster",
  push_hero_on_sword = true,
  movement_create = random_movement_create
})

function random_movement_create()
  
  local m = sol.movement.create("random")
  m:set_smooth(true)
  return m
end


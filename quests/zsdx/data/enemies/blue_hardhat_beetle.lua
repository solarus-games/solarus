-- Blue Hardhat Beetle

sol.main.do_file("enemies/generic_towards_hero")
set_properties({
  sprite = "enemies/blue_hardhat_beetle",
  life = 5,
  damage = 4,
  normal_speed = 32,
  faster_speed = 48,
  hurt_style = "monster",
  push_hero_on_sword = true,
  movement_create = function()
    local m = sol.movement.create("random")
    m:set_smooth(true)
    return m
  end
})


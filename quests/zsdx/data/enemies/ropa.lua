-- Ropa

sol.main.do_file("enemies/generic_towards_hero")
set_properties({
  sprite = "enemies/ropa",
  life = 6,
  damage = 4,
  normal_speed = 32,
  faster_speed = 32,
  hurt_style = "monster",
  movement_create = function()
    return sol.movement.create("random")
  end
})


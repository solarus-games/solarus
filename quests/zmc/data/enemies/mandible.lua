local enemy = ...

-- Mandible.

sol.main.load_file("enemies/generic_towards_hero")(enemy)
enemy:set_properties({
  sprite = "enemies/mandible",
  life = 3,
  damage = 2,
  hurt_style = "monster"
})


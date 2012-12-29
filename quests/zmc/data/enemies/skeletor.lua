local enemy = ...

-- Skeletor.

sol.main.load_file("enemies/generic_towards_hero")(enemy)
enemy:set_properties({
  sprite = "enemies/skeletor",
  life = 3,
  damage = 2
})


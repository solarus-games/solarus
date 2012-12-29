local enemy = ...

-- Minillosaur waiting in eggs for the hero to drop by.

sol.main.load_file("enemies/generic_waiting_for_hero")(enemy)
enemy:set_properties({
  sprite = "enemies/minillosaur_egg_fixed",
  life = 4,
  damage = 2,
  normal_speed = 32,
  faster_speed = 48,
  hurt_style = "normal",
  push_hero_on_sword = false,
  pushed_when_hurt = true,
  asleep_animation = "egg",
  awaking_animation = "egg_breaking",
  normal_animation = "walking",
  obstacle_behavior = "flying",
  awakening_sound = "stone"
})


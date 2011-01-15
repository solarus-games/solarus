-- Tentacle: a basic enemy that follows the hero

-- Properties
life = 1
damage = 2
sprite = "enemies/tentacle"
size = {16, 16}
origin = {8, 13}

-- The enemy appears: set its properties
function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/tentacle")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  m = sol.main.path_finding_movement_create(32)
  sol.enemy.start_movement(m)
end

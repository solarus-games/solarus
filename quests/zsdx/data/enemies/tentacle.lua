-- Tentacle: a basic enemy that follows the hero

-- The enemy appears: set its properties
function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/tentacle")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
end

function event_restart()

  local m = sol.movement.create("path_finding")
  m:set_speed(32)
  sol.enemy.start_movement(m)
end


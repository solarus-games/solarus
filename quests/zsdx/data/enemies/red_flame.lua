-- A red flame thrown by another enemy

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/red_flame")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_layer_independent_collisions(true)
end

function event_restart()

  -- by default, go to where the hero is now
  local x, y = sol.map.hero_get_position()
  x = x + math.random(-32, 32)
  y = y + math.random(-32, 32)
  local m = sol.movement.create("target")
  m:set_speed(144)
  m:set_target(x, y)
  m:set_ignore_obstacles(true)
  sol.enemy.start_movement(m)
  sol.main:start_timer(2000, event_movement_finished)
end

function event_movement_finished(movement)

  -- create a circle of fire
  local son_name = sol.enemy.get_name() .. "_circle"
  sol.enemy.create_son(son_name, "fire_circle", 0, 0)
  sol.map.enemy_remove(sol.enemy.get_name())
end

function event_message_received(src_enemy, message)

  -- the message is the angle to take
  local angle = tonumber(message)
  local m = sol.movement.create("straight")
  m:set_speed(144)
  m:set_angle(angle)
  m:set_ignore_obstacles(true)
  m:set_max_distance(320)
  sol.enemy.start_movement(m)
end


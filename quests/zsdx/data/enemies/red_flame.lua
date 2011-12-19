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
  local m = sol.main.target_movement_create(144, x, y)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
  sol.main.timer_start(event_movement_finished, 2000)
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
  local m = sol.main.straight_movement_create(144, angle)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.main.movement_set_property(m, "max_distance", 320)
  sol.enemy.start_movement(m)
end


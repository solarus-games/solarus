-- A blue flame thrown by another enemy

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(8)
  sol.enemy.create_sprite("enemies/blue_flame")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_obstacle_behavior("flying")
end

function event_obstacle_reached()

  sol.map.enemy_remove(sol.enemy.get_name())
end

function event_message_received(src_enemy, message)

  -- the message is the angle to take
  local angle = tonumber(message)
  local m = sol.main.straight_movement_create(192, angle)
  sol.enemy.start_movement(m)
end


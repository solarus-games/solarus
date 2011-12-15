-- A fire bat thrown by Ganon

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(12)
  sol.enemy.create_sprite("enemies/fire_bat")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 8)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", 1)
  sol.enemy.set_attack_consequence("arrow", 1)
  sol.enemy.set_attack_consequence("hookshot", 1)
  sol.enemy.set_attack_consequence("boomerang", 1)
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_layer_independent_collisions(true)
end

function event_restart()

  if sol.enemy.get_movement() == nil then
    go_hero()
  end
end

function event_movement_finished(movement)

  sol.map.enemy_remove(sol.enemy.get_name())
end

function go_hero()

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  local angle = sol.main.get_angle(x, y, hero_x, hero_y)
  local m = sol.main.straight_movement_create(192, angle)
  sol.main.movement_set_property(m, "max_distance", 320)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
end

function circle()

  -- make a circle around the father
  local m = sol.main.circle_movement_create(7, sol.enemy.get_father(), 48)
  sol.main.movement_set_property(m, "center_dy", -21)
  sol.main.movement_set_property(m, "initial_angle", 90)
  sol.main.movement_set_property(m, "angle_speed", 150)
  sol.enemy.start_movement(m)
end

function event_message_received(src_enemy, message)

  if message == "circle" then
    circle()
  elseif message == "go_hero" then
    go_hero()
  end
end


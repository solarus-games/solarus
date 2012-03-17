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
  local m = sol.movement.straight_movement_create(192, angle)
  m:set_property("max_distance", 320)
  m:set_property("ignore_obstacles", true)
  sol.enemy.start_movement(m)
end

function go_circle()

  -- make a circle around the father
  local m = sol.movement.circle_movement_create(7, sol.enemy.get_father(), 48)
  m:set_property("center_dy", -21)
  m:set_property("initial_angle", 90)
  m:set_property("angle_speed", 150)
  sol.enemy.start_movement(m)
end

function event_message_received(src_enemy, message)

  if message == "circle" then
    go_circle()
  elseif message == "go_hero" then
    go_hero()
  else
    local delay = tonumber(message:match("^go_hero ([0-9]*)$"))
    if delay then
      sol.timer.start(delay, function()
	go_hero()
      end)
    end
  end
end


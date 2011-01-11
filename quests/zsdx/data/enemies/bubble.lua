life = 1
damage = 2
magic_damage = 4
sprite = "enemies/bubble"
size = {12, 12}
origin = {6, 6}
invincible = true

function event_appear()
  m = sol.main.rectilinear_movement_create(32)
  sol.enemy.start_movement(m)
end

function event_restart()
  direction8 = math.random(4) * 2 + 1
  go(direction8)
end

function event_obstacle_reached()
  -- TODO: bounce
end

function go(direction8)
  m = sol.enemy.get_movement()
  sol.main.movement_set_property("speed", 80)
  sol.main.movement.set_property("angle", direction8 * math.pi / 4)
end

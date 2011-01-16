-- Bubble: an invincible enemy that moves in diagonal directions and bounces against walls.
-- It removes life and magic points from the hero.

last_direction8 = 0

-- The enemy appears: set its properties
function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(2)
  sol.enemy.set_magic_damage(4)
  sol.enemy.create_sprite("enemies/bubble")
  sol.enemy.set_size(12, 12)
  sol.enemy.set_origin(6, 6)
  sol.enemy.set_invincible()
  m = sol.main.rectilinear_movement_create(80, 0)
  sol.enemy.start_movement(m)
end

-- The enemy was stopped for some reason and should restart
function event_restart()

  direction8 = math.random(4) * 2 - 1
  go(direction8)
end

-- An obstacle is reached: make the Bubble bounce
function event_obstacle_reached()

  print("bubble "..sol.enemy.get_name()..": obstacle reached")
  dxy = {
    { x =  1, y =  0},
    { x =  1, y = -1},
    { x =  0, y = -1},
    { x = -1, y = -1},
    { x = -1, y =  0},
    { x = -1, y =  1},
    { x =  0, y =  1},
    { x =  1, y =  1}
  }

  -- last_direction8 was the current direction, try the three other diagonal directions
  try1 = (last_direction8 + 2) % 8;
  try2 = (last_direction8 + 6) % 8;
  try3 = (last_direction8 + 4) % 8;

  if not sol.main.movement_test_obstacles(m, dxy[try1 + 1].x, dxy[try1 + 1].y) then

    x, y = sol.enemy.get_position()
    go(try1)
  elseif not sol.main.movement_test_obstacles(m, dxy[try2 + 1].x, dxy[try2 + 1].y) then

    x, y = sol.enemy.get_position()
    go(try2)
  else
    go(try3)
  end

end

-- Makes the Bubble go towards a diagonal direction (1, 3, 5 or 7)
function go(direction8)

  m = sol.enemy.get_movement()
  sol.main.movement_set_property(m, "speed", 80)
  sol.main.movement_set_property(m, "angle", direction8 * math.pi / 4)
  last_direction8 = direction8
end



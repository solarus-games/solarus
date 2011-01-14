-- Bubble: an invincible enemy that moves in diagonal directions and bounces against walls.
-- It removes life and magic points to the hero.

-- Properties
life = 1
damage = 2
magic_damage = 4
sprite = "enemies/bubble"
size = {12, 12}
origin = {6, 6}
invincible = true

-- The enemy appears: create its movement
function event_appear()
  m = sol.main.rectilinear_movement_create(32, 0)
  sol.enemy.start_movement(m)
end

-- The enemy was stopped for some reason and should restart
function event_restart()
  direction8 = math.random(4) * 2 + 1
  go(direction8)
end

-- An obstacle is reached: make the Bubble bounce
function event_obstacle_reached()

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

  m = sol.enemy.get_movement()
  angle = sol.main.movement_get_property(m, "angle")
  direction8 = angle * 4 / math.pi -- this was the current direction

  -- try the three other diagonal directions
  try1 = (direction8 + 2) % 8;
  try2 = (direction8 + 6) % 8;
  try3 = (direction8 + 4) % 8;

  if not sol.main.movement_test_obstacles(m, dxy[try1].x, dxy[try1].y) then
    go(try1)
  elseif not sol.main.movement_test_obstacles(m, dxy[try2].x, dxy[try2].y) then
    go(try2)
  else
    go(try3)
  end

end

-- Makes the Bubble go towards a diagonal direction (1, 3, 5 or 7)
function go(direction8)
  m = sol.enemy.get_movement()
  sol.main.movement_set_property(m, "speed", 80)
  sol.main.movement.set_property(m, "angle", direction8 * math.pi / 4)
end


local enemy = ...

-- Bubble: an invincible enemy that moves in diagonal directions
-- and bounces against walls.
-- It removes life and magic points from the hero.

local last_direction8 = 0

-- The enemy appears: set its properties.
function enemy:on_created()

  self:set_life(1)
  self:set_damage(2)
  self:set_magic_damage(4)
  self:create_sprite("enemies/bubble")
  self:set_size(12, 12)
  self:set_origin(6, 6)
  self:set_can_hurt_hero_running(true)
  self:set_invincible()
end

-- The enemy was stopped for some reason and should restart.
function enemy:on_restarted()

  local direction8 = math.random(4) * 2 - 1
  self:go(direction8)
end

-- An obstacle is reached: make the Bubble bounce.
function enemy:on_obstacle_reached()

  local dxy = {
    { x =  1, y =  0},
    { x =  1, y = -1},
    { x =  0, y = -1},
    { x = -1, y = -1},
    { x = -1, y =  0},
    { x = -1, y =  1},
    { x =  0, y =  1},
    { x =  1, y =  1}
  }

  -- The current direction is last_direction8:
  -- try the three other diagonal directions.
  local try1 = (last_direction8 + 2) % 8
  local try2 = (last_direction8 + 6) % 8
  local try3 = (last_direction8 + 4) % 8

  if not self:test_obstacles(dxy[try1 + 1].x, dxy[try1 + 1].y) then
    self:go(try1)
  elseif not self:test_obstacles(dxy[try2 + 1].x, dxy[try2 + 1].y) then
    self:go(try2)
  else
    self:go(try3)
  end
end

-- Makes the Bubble go towards a diagonal direction (1, 3, 5 or 7).
function enemy:go(direction8)

  local m = sol.movement.create("straight")
  m:set_speed(80)
  m:set_angle(direction8 * math.pi / 4)
  self:start_movement(m)
  last_direction8 = direction8
end



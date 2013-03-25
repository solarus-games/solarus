local enemy = ...

-- A ball and its chain managed by another enemy.
-- The ball is controlled by a circular movement and
-- the chain is a sprite that automatically fits the space between the other enemy and the ball.
-- They usually disappear when the enemy is killed.

local nb_links = 10
local link_sprite = nil
local link_xy = {
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0},
  {x = 0, y = 0}
}
local center_enemy = nil          -- The enemy this chain and ball is attached to.
local center_xy = {x = 0, y = 0}  -- Center point of the circles, relative to the center enemy if any.

function enemy:on_created()

  self:set_life(1)
  self:set_damage(3)
  self:create_sprite("enemies/chain_and_ball")
  self:set_size(16, 16)
  self:set_origin(8, 8)
  self:set_invincible()

  -- Create a second sprite that stays independent of the enemy.
  link_sprite = sol.sprite.create("enemies/chain_and_ball")
  link_sprite:set_animation("chain")

  -- Initialize the links of the chain.
  for i = 1, nb_links do
    link_xy[i].x = 0
    link_xy[i].y = 0
  end
end

function enemy:set_center_enemy(other)

  -- Get the difference of coordinates between me and the center of circles.
  local x, y = self:get_position()
  local other_x, other_y = other:get_position()
  center_xy.x, center_xy.y = x - other_x, y - other_y
  center_enemy = other
end

function enemy:on_pre_draw()

  for i = 1, nb_links do
    self:get_map():draw_sprite(link_sprite, link_xy[i].x, link_xy[i].y)
  end
end

function enemy:on_position_changed(x, y)

  -- Recalculate the chain position.
  if center_enemy ~= nil then
    -- The center is relative to that enemy.
    local x, y = center_enemy:get_position()
    local x1, y1 = x + center_xy.x, y + center_xy.y
    local x2, y2 = self:get_position()

    for i = 1, nb_links do
      link_xy[i].x = x1 + (x2 - x1) * (i - 1) / nb_links
      link_xy[i].y = y1 + (y2 - y1) * (i - 1) / nb_links
    end
  end
end

function enemy:on_restarted()

  local m = sol.movement.create("circle")
  m:set_center(center_enemy, center_xy.x, center_xy.y)
  m:set_radius(56)
  m:set_radius_speed(50)
  m:set_max_rotations(4)
  m:set_loop(2000)
  m:set_angle_speed(360)
  m:start(self)
end


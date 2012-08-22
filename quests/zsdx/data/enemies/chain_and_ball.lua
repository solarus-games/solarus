local enemy = ...

-- A ball and its chain, usually controlled by another enemy.
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
local father_name = ""                  -- name of the enemy the chain and ball is attached to if any
local center_xy = {x = 0, y = 0}        -- center point of the circles, relative to the father enemy if any, absolute otherwise

function enemy:on_appear()

  -- set the properties
  self:set_life(1)
  self:set_damage(3)
  self:create_sprite("enemies/chain_and_ball")
  self:set_size(16, 16)
  self:set_origin(8, 8)
  self:set_invincible()

  -- create a second sprite that stays in the script
  link_sprite = sol.sprite.create("enemies/chain_and_ball")
  link_sprite:set_animation("chain")

  -- initialize the links of the chain
  for i = 1, nb_links do
    link_xy[i].x = 0
    link_xy[i].y = 0
  end

  -- get the difference of coordinates between me and my father
  father_name = self:get_father()
  if father_name ~= "" then
    local x, y = self:get_position()
    local father_x, father_y = self:get_map():enemy_get_position(father_name)
    center_xy.x, center_xy.y = x - father_x, y - father_y
  end
end

function enemy:on_pre_draw()

  for i = 1, nb_links do
    self:get_map():draw_sprite(link_sprite, link_xy[i].x, link_xy[i].y)
  end
end

function enemy:on_position_changed(x, y)

  -- recalculate the chain position
  local x1, y1
  if father_name ~= "" then
    -- the center is relative to the father
    local x, y = self:get_map():enemy_get_position(father_name)
    x1, y1 = x + center_xy.x, y + center_xy.y
  else
    -- the center is absolute
    x1, y1 = center_xy
  end
  local x2, y2 = self:get_position()

  for i = 1, nb_links do
    link_xy[i].x = x1 + (x2 - x1) * (i - 1) / nb_links
    link_xy[i].y = y1 + (y2 - y1) * (i - 1) / nb_links
  end
end

function enemy:on_restart()

  local m = sol.movement.create("circle")
  m:set_center(7, father_name, center_xy.x, center_xy.y)
  m:set_radius(56)
  m:set_radius_speed(50)
  m:set_max_rotations(4)
  m:set_loop(2000)
  m:set_angle_speed(360)
  self:start_movement(m)
end


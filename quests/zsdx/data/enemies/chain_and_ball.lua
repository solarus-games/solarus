-- A ball and its chain, usually controlled by another enemy.
-- The ball is controlled by a circular movement and
-- the chain is a sprite that automatically fits the space between the other enemy and the ball.
-- They usually disappear when the enemy is killed.

nb_links = 10
link_sprite = nil
link_xy = {
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
father_name = ""                  -- name of the enemy the chain and ball is attached to if any
center_xy = {x = 0, y = 0}        -- center point of the circles, relative to the father enemy if any, absolute otherwise

function event_appear()

  -- set the properties
  sol.enemy.set_life(1)
  sol.enemy.set_damage(3)
  sol.enemy.create_sprite("enemies/chain_and_ball")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 8)
  sol.enemy.set_invincible()

  -- create a second sprite that stays in the script
  link_sprite = sol.main.sprite_create("enemies/chain_and_ball")
  sol.main.sprite_set_animation(link_sprite, "chain")

  -- initialize the links of the chain
  for i = 1, nb_links do
    link_xy[i].x = 0
    link_xy[i].y = 0
  end

  -- get the difference of coordinates between me and my father
  father_name = sol.enemy.get_father()
  if father_name ~= "" then
    local x, y = sol.enemy.get_position()
    local father_x, father_y = sol.map.enemy_get_position(father_name)
    center_xy.x, center_xy.y = x - father_x, y - father_y
  end
end

function event_pre_display()

  for i = 1, nb_links do
    sol.map.sprite_display(link_sprite, link_xy[i].x, link_xy[i].y)
  end
end

function event_position_changed(x, y)

  -- recalculate the chain position
  local x1, y1
  if father_name ~= "" then
    -- the center is relative to the father
    local x, y = sol.map.enemy_get_position(father_name)
    x1, y1 = x + center_xy.x, y + center_xy.y;
  else
    -- the center is absolute
    x1, y1 = center_xy
  end
  local x2, y2 = sol.enemy.get_position();

  for i = 1, nb_links do
    link_xy[i].x = x1 + (x2 - x1) * (i - 1) / nb_links;
    link_xy[i].y = y1 + (y2 - y1) * (i - 1) / nb_links;
  end
end

function event_enabled()

  local m = sol.main.circle_movement_create(7, father_name, 56)
  sol.main.movement_set_property(m, "center_dx", center_xy.x)
  sol.main.movement_set_property(m, "center_dy", center_xy.y)
  sol.main.movement_set_property(m, "radius_speed", 50)
  sol.main.movement_set_property(m, "max_rotations", 4)
  sol.main.movement_set_property(m, "loop", 2000)
  sol.main.movement_set_property(m, "angle_speed", 360)
  sol.enemy.start_movement(m)
end



-- A ball and its chain, usually controlled by another enemy.
-- The ball is controlled by a circular movement and
-- the chain is a sprite that automatically fits the space between the other enemy and the ball.
-- They usually disappear when the enemy is killed.

-- Properties
life = 1
damage = 2
sprite = "enemies/chain_and_ball"
size = {16, 16}
origin = {8, 8}
invincible = true
display_in_y_order = false

-- Specific
nb_links = 10
link_sprite = nil
link_xy = {x = 0, y = 0}
creator_name = ""                  -- name of the enemy the chain and ball is attached to
creator_dxy = {x = 0, y = 0};      -- center point of the circles, relative to the enemy

function event_appear()

  -- set the sprite
  link_sprite = sol.main.sprite_create("enemies/chain_and_ball")
  sol.main.sprite_set_animation("chain")

  -- initialize the links of the chain
  for i = 0, nb_links - 1 do
    link_xy[i].x = 0
    link_xy[i].y = 0
  end

  -- get the creator
  creator_name = sol.enemy.get_creator()
  x, y = sol.enemy.get_creator_relative_position()
  center_enemy_dxy.x = x
  center_enemy_dxy.y = y
end

function event_display()

  for i = 0, nb_links - 1 do
    sol.map.sprite_display(link_sprite, link_xy[i].x, link_xy[i].y)
  end
end

function event_position_changed(x, y)

  -- recalculate the chain position
  if creator_name != "" then

    x, y = sol.map.enemy_get_position(creator_name)
    x1 = x + center_enemy_dxy.x;
    y1 = y + center_enemy_dxy.y;
    x2 = get_x();
    y2 = get_y();


  for i = 0, nb_links - 1 do
    link_xy[i].x = x1 + (x2 - x1) * i / nb_links;
    link_xy[i].y = y1 + (y2 - y1) * i / nb_links;
  end
end

function event_enabled()

  m = sol.main.circle_movement_create("enemy", creator_name, 64)
  sol.main.movement_set_property(m, "center_dx", -16)
  sol.main.movement_set_property(m, "center_dy", -33)
  sol.main.movement_set_property(m, "radius_speed", 50)
  sol.main.movement_set_property(m, "max_rotations", 4)
  sol.main.movement_set_property(m, "loop", 2000)
  sol.main.movement_set_property(m, "angle_speed", 360)
  sol.enemy.start_movement(m)
end



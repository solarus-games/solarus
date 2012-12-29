local enemy = ...

local eye = nil
local current_xy = {}
local ball_sprite = nil

function enemy:on_created()

  self:set_life(1)
  self:set_damage(6)
  self:create_sprite("enemies/flora_gohma")
  self:set_size(80, 80)
  self:set_origin(40, 64)
  self:set_hurt_style("boss")
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("hookshot", "protected")

  -- Create the eye.
  local my_name = self:get_name()
  eye = self:create_enemy(my_name .. "_eye", "flora_gohma_eye", 0, -112)
  eye.body = self

  -- Create the balls of the neck.
  ball_sprite = sol.sprite.create("enemies/flora_gohma_eye")
  ball_sprite:set_animation("ball")
end

function enemy:on_restarted()

  local m = sol.movement.create("random")
  m:set_speed(16)
  m:set_max_distance(24)
  self:start_movement(m)
  current_xy.x, current_xy.y = self:get_position()
end

function enemy:on_position_changed(x, y)

  if eye:exists() then
    -- The body has just moved: do the same movement to the eye.
    local dx = x - current_xy.x
    local dy = y - current_xy.y
    local eye_x, eye_y = eye:get_position()
    eye:set_position(eye_x + dx, eye_y + dy)
  end
  current_xy.x, current_xy.y = x, y
end

function enemy:on_pre_draw()

  if eye:exists() then
    local x, y = self:get_position()
    local eye_x, eye_y = eye:get_position()
    self:display_balls(x, y - 32, eye_x, eye_y - 16)
  end
end

function enemy:display_balls(x1, y1, x2, y2)

  local nb_balls = 10
  local x = x1
  local y = y1
  local x_inc = (x2 - x1) / (nb_balls - 1)
  local y_inc = (y2 - y1) / (nb_balls - 1)
  for i = 1, nb_balls do
    self:get_map():draw_sprite(ball_sprite, x, y)
    x = x + x_inc
    y = y + y_inc
  end
end


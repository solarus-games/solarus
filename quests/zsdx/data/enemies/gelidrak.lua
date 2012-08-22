local enemy = ...

-- Gelidrak: an ice dragon from Newlink

local head = nil
local head_ball_sprite = nil
local head_vulnerable = false
local tail = nil
local tail_ball_sprite = nil
local tail_retracted = false
local initial_xy = {}
local current_xy = {}

function enemy:on_created()

  self:set_life(1000000)
  self:set_damage(12)
  self:create_sprite("enemies/gelidrak")
  self:set_size(240, 112)
  self:set_origin(120, 88)
  self:set_obstacle_behavior("flying")
  self:set_layer_independent_collisions(true)
  self:set_push_hero_on_sword(true)

  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_pushed_back_when_hurt(false)

  -- create the head
  local my_name = self:get_name()
  head = my_name .. "_head"
  self:create_son(head, "gelidrak_head", 0, 48)
  head_ball_sprite = sol.sprite.create("enemies/gelidrak")
  head_ball_sprite:set_animation("head_ball")

  -- create the tail
  local my_name = self:get_name()
  tail = my_name .. "_tail"
  self:create_son(tail, "gelidrak_tail", 0, -112)
  tail_ball_sprite = sol.sprite.create("enemies/gelidrak")
  tail_ball_sprite:set_animation("tail_ball")

  initial_xy.x, initial_xy.y = self:get_position()
end

function enemy:on_restarted()

  local sprite = self:get_sprite()
  if head_vulnerable then
    sprite:set_animation("fast")
    self:set_can_attack(false)
  else
    sprite:set_animation("walking")
    local m = sol.movement.create("random")
    m:set_speed(32)
    self:start_movement(m)
  end
  current_xy.x, current_xy.y = self:get_position()
end

function enemy:on_pre_draw()

  if not self:get_map():enemy_is_dead(tail) then
    local x, y = self:get_position()
    local tail_x, tail_y = self:get_map():enemy_get_position(tail)
    self:display_balls(tail_ball_sprite, 6, x, y - 48, tail_x, tail_y)
  end
end

function enemy:on_post_draw()

  if not self:get_map():enemy_is_dead(head) then
    local x, y = self:get_position()
    local head_x, head_y = self:get_map():enemy_get_position(head)
    self:display_balls(head_ball_sprite, 7, x, y + 8, head_x, head_y - 16)
  end
end

function enemy:display_balls(ball_sprite, nb_balls, x1, y1, x2, y2)

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

function enemy:on_position_changed(x, y)

  -- the body has just moved: do the same movement to the head and the tail
  local dx = x - current_xy.x
  local dy = y - current_xy.y
  local tail_x, tail_y = self:get_map():enemy_get_position(tail)
  self:get_map():enemy_set_position(tail, tail_x + dx, tail_y + dy)
  local head_x, head_y = self:get_map():enemy_get_position(head)
  self:get_map():enemy_set_position(head, head_x + dx, head_y + dy)
  current_xy.x, current_xy.y = x, y
end

function enemy:on_message_received(src_enemy, message)

  if self:get_life() <= 0 then
    -- ignore messages once I'm dying
    return
  end

  if src_enemy == tail then
    
    if message == "hurt" then
      -- the hero just hurt my tail
      head_vulnerable = true
      self:set_can_attack(false)
      local sprite = self:get_sprite()
      sprite:set_animation("fast")
      self:stop_movement()
      self:send_message(head, "vulnerable")
    elseif message == "recovering" then
      -- the tail starts recovering its normal position
      self:send_message(head, "recover")
    elseif message == "recovered" then
      -- the tail has recovered its normal position
      self:on_restarted()
    end

  elseif src_enemy == head then

    if message == "hurt" then
      -- the hero just hurt my head
    elseif message == "dying" then
      -- I'm dying: remove the tail
      self:get_map():enemy_remove(tail)
    elseif message == "dead" then
      -- the head is dead: make the body die too
      self:hurt(self:get_life())
    elseif message == "recovered" then
      -- my head has just stopped being vulnerable
      head_vulnerable = false
      self:set_can_attack(true)
      self:on_restarted()
    end
  end
end


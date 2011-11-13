-- Gelidrak: an ice dragon from Newlink

head = nil
head_ball_sprite = nil
head_vulnerable = false
tail = nil
tail_ball_sprite = nil
tail_retracted = false
initial_xy = {}
current_xy = {}

function event_appear()

  sol.enemy.set_life(1000000)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/gelidrak")
  sol.enemy.set_size(240, 112)
  sol.enemy.set_origin(120, 88)
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_layer_independent_collisions(true)

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_pushed_back_when_hurt(false)

  -- create the head
  local my_name = sol.enemy.get_name()
  head = my_name .. "_head"
  sol.enemy.create_son(head, "gelidrak_head", 0, 48)
  head_ball_sprite = sol.main.sprite_create("enemies/gelidrak")
  sol.main.sprite_set_animation(head_ball_sprite, "head_ball")

  -- create the tail
  local my_name = sol.enemy.get_name()
  tail = my_name .. "_tail"
  sol.enemy.create_son(tail, "gelidrak_tail", 0, -112)
  tail_ball_sprite = sol.main.sprite_create("enemies/gelidrak")
  sol.main.sprite_set_animation(tail_ball_sprite, "tail_ball")

  initial_xy.x, initial_xy.y = sol.enemy.get_position()
end

function event_restart()

  local sprite = sol.enemy.get_sprite()
  if head_vulnerable then
    sol.main.sprite_set_animation(sprite, "fast")
    sol.enemy.set_can_attack(false)
  else
    sol.main.sprite_set_animation(sprite, "walking")
    local m = sol.main.random_movement_create(32)
    sol.enemy.start_movement(m)
  end
  current_xy.x, current_xy.y = sol.enemy.get_position()
end

function event_pre_display()

  if not sol.map.enemy_is_dead(tail) then
    local x, y = sol.enemy.get_position()
    local tail_x, tail_y = sol.map.enemy_get_position(tail)
    display_balls(tail_ball_sprite, 6, x, y - 48, tail_x, tail_y)
  end
end

function event_post_display()

  if not sol.map.enemy_is_dead(head) then
    local x, y = sol.enemy.get_position()
    local head_x, head_y = sol.map.enemy_get_position(head)
    display_balls(head_ball_sprite, 7, x, y + 8, head_x, head_y - 16)
  end
end

function display_balls(ball_sprite, nb_balls, x1, y1, x2, y2)

  local x = x1
  local y = y1
  local x_inc = (x2 - x1) / (nb_balls - 1)
  local y_inc = (y2 - y1) / (nb_balls - 1)
  for i = 1, nb_balls do
    sol.map.sprite_display(ball_sprite, x, y)
    x = x + x_inc
    y = y + y_inc
  end
end

function event_position_changed(x, y)

  -- the body has just moved: do the same movement to the head and the tail
  local dx = x - current_xy.x
  local dy = y - current_xy.y
  local tail_x, tail_y = sol.map.enemy_get_position(tail)
  sol.map.enemy_set_position(tail, tail_x + dx, tail_y + dy)
  local head_x, head_y = sol.map.enemy_get_position(head)
  sol.map.enemy_set_position(head, head_x + dx, head_y + dy)
  current_xy.x, current_xy.y = x, y
end

function event_message_received(src_enemy, message)

  if sol.enemy.get_life() <= 0 then
    -- ignore messages once I'm dying
    return
  end

  if src_enemy == tail then
    
    if message == "hurt" then
      -- the hero just hurt my tail
      head_vulnerable = true
      sol.enemy.set_can_attack(false)
      local sprite = sol.enemy.get_sprite()
      sol.main.sprite_set_animation(sprite, "fast")
      sol.enemy.stop_movement()
      sol.enemy.send_message(head, "vulnerable")
    elseif message == "recovering" then
      -- the tail starts recovering its normal position
      sol.enemy.send_message(head, "recover")
    elseif message == "recovered" then
      -- the tail has recovered its normal position
      event_restart()
    end

  elseif src_enemy == head then

    if message == "hurt" then
      -- the hero just hurt my head
      sol.enemy.hurt(1)
    elseif message == "dead" then
      sol.enemy.hurt(sol.enemy.get_life())
      -- I'm dying: remove the tail
      sol.map.enemy_remove(tail)
    elseif message == "recovered" then
      -- my head has just stopped being vulnerable
      head_vulnerable = false
      sol.enemy.set_can_attack(true)
      event_restart()
    end
  end
end


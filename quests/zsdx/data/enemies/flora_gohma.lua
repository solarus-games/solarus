local eye = nil
local current_xy = {}
local ball_sprite = nil

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(6)
  sol.enemy.create_sprite("enemies/flora_gohma")
  sol.enemy.set_size(80, 80)
  sol.enemy.set_origin(40, 64)
  sol.enemy.set_hurt_style("boss")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")

  -- create the eye
  local my_name = sol.enemy.get_name()
  eye = my_name.."_eye"
  sol.enemy.create_son(eye, "flora_gohma_eye", 0, -112)

  -- create the balls of the neck
  ball_sprite = sol.main.sprite_create("enemies/flora_gohma_eye")
  sol.main.sprite_set_animation(ball_sprite, "ball")
end

function event_restart()

  local m = sol.main.random_movement_create(16)
  sol.main.movement_set_property(m, "max_distance", 24)
  sol.enemy.start_movement(m)
  current_xy.x, current_xy.y = sol.enemy.get_position()
end

function event_position_changed(x, y)

  if not sol.map.enemy_is_dead(eye) then
    -- the body has just moved: do the same movement to the eye
    local dx = x - current_xy.x
    local dy = y - current_xy.y
    local eye_x, eye_y = sol.map.enemy_get_position(eye)
    sol.map.enemy_set_position(eye, eye_x + dx, eye_y + dy)
  end
  current_xy.x, current_xy.y = x, y
end

function event_pre_display()

  if not sol.map.enemy_is_dead(eye) then
    local x, y = sol.enemy.get_position()
    local eye_x, eye_y = sol.map.enemy_get_position(eye)
    display_balls(x, y - 32, eye_x, eye_y - 16)
  end
end

function display_balls(x1, y1, x2, y2)

  local nb_balls = 10
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

function event_message_received(src_enemy, message)

  if src_enemy == eye then
    
    if message == "dying" then
      -- the eye is dying: stop moving
      sol.enemy.stop_movement()
    elseif message == "dead" then
      -- the eye is dead: I'm gonna have to die too
      sol.enemy.hurt(1)
    end
  end
end


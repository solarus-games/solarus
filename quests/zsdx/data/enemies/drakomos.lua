head_1 = nil
head_2 = nil
ball_sprite = nil

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/drakomos")
  sol.enemy.set_size(128, 80)
  sol.enemy.set_origin(64, 64)
  sol.enemy.set_invincible()
  sol.enemy.set_displayed_in_y_order(false)

  -- create the heads
  local my_name = sol.enemy.get_name()
  head_1 = my_name.."_head_1"
  head_2 = my_name.."_head_2"
  sol.enemy.create_son(head_1, "drakomos_head", -76, 40)
  sol.enemy.create_son(head_2, "drakomos_head", 76, 40)
  ball_sprite = sol.main.sprite_create("enemies/drakomos")
  sol.main.sprite_set_animation(ball_sprite, "ball")
end

function event_post_display()

  local x, y = sol.enemy.get_position()
  local x2, y2 = sol.map.enemy_get_position(head_1)
  display_balls(x - 22, y - 15, x2, y2 - 12)
  x2, y2 = sol.map.enemy_get_position(head_2)
  display_balls(x + 22, y - 15, x2, y2 - 12)
end

function display_balls(x1, y1, x2, y2)

  local nb_balls = 5
  for i = 1, nb_balls do
    local x = x1 + (x2 - x1) * i / nb_balls
    local y = y1 + (y2 - y1) * i / nb_balls
    sol.map.sprite_display(ball_sprite, x, y)
  end
end


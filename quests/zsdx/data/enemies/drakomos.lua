local enemy = ...

local head_1 = nil
local head_2 = nil
local ball_sprite = nil
local killing = false

function enemy:on_created()

  self:set_life(1)
  self:set_damage(8)
  self:create_sprite("enemies/drakomos")
  self:set_size(128, 80)
  self:set_origin(64, 64)
  self:set_invincible()

  -- create the heads
  local my_name = self:get_name()
  head_1 = my_name.."_head_1"
  head_2 = my_name.."_head_2"
  self:create_son(head_1, "drakomos_head", -76, 40)
  self:create_son(head_2, "drakomos_head", 76, 40)
  ball_sprite = sol.sprite.create("enemies/drakomos")
  ball_sprite:set_animation("ball")
end

function enemy:on_update()

  if self:get_map():enemy_is_dead(head_1)
    and self:get_map():enemy_is_dead(head_2)
    and self:get_life() > 0
    and not killing then

    killing = true
    self:hurt(1)
    self:get_map():enemy_remove_group(head_1)
    self:get_map():enemy_remove_group(head_2)
    self:get_map():enemy_remove_group("spawner")
  end
end

function enemy:on_post_draw()

  local x, y = self:get_position()

  if not self:get_map():enemy_is_dead(head_1) then
    local x2, y2 = self:get_map():enemy_get_position(head_1)
    self:display_balls(x - 22, y - 15, x2, y2 - 12)
  end

  if not self:get_map():enemy_is_dead(head_2) then
    local x2, y2 = self:get_map():enemy_get_position(head_2)
    self:display_balls(x + 22, y - 15, x2, y2 - 12)
  end
end

function enemy:display_balls(x1, y1, x2, y2)

  local nb_balls = 5
  for i = 1, nb_balls do
    local x = x1 + (x2 - x1) * i / nb_balls
    local y = y1 + (y2 - y1) * i / nb_balls
    self:get_map():draw_sprite(ball_sprite, x, y)
  end
end


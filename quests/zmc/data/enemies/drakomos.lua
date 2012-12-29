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

  -- Create the heads.
  local my_name = self:get_name()
  head_1 = self:create_enemy(my_name .. "head_1", "drakomos_head", -76, 40)
  head_2 = self:create_enemy(my_name .. "head_2", "drakomos_head", 76, 40)
  ball_sprite = sol.sprite.create("enemies/drakomos")
  ball_sprite:set_animation("ball")
end

function enemy:on_update()

  if not head_1:exists()
    and not head_2:exists()
    and self:get_life() > 0
    and not killing then

    killing = true
    self:hurt(1)
    self:get_map():remove_entities(head_1:get_name())
    self:get_map():remove_entities(head_2:get_name())
    self:get_map():remove_entities("spawner")
  end
end

function enemy:on_post_draw()

  local x, y = self:get_position()

  if head_1:exists() then
    local x2, y2 = head_1:get_position()
    self:display_balls(x - 22, y - 15, x2, y2 - 12)
  end

  if head_2:exists() then
    local x2, y2 = head_2:get_position()
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


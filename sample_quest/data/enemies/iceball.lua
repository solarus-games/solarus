local enemy = ...
local map = enemy:get_map()

local ice_duration = 2000 -- Duration of the frozen state on the hero when hit by the iceball.
local frost_damage = 4
local default_speed = 48

function enemy:on_created()
  sprite = enemy:create_sprite("enemies/" .. enemy:get_breed())
  enemy:set_life(1)
  enemy:set_damage(0)
end

function enemy:on_restarted()
  self:start_movement()
end

function enemy:start_movement(angle, speed)
  local a = angle
  if not a then a = self:get_angle(map:get_hero()) end
  local s = speed or default_speed
  local m = sol.movement.create("straight")
  m:set_angle(a)
  m:set_speed(s)
  m:set_smooth(false)
  m:start(self)
  -- Kill the fireball if it reaches an obstacle.
  function m:on_obstacle_reached() enemy:set_life(0) end
  function m:on_finished() enemy:set_life(0) end
end

-- Freeze the hero if there is a collision.
function enemy:on_attacking_hero(hero, enemy_sprite)
  -- Freeze hero and create ice block.
  hero:freeze()
  hero:set_invincible(true, ice_duration)
  sol.audio.play_sound("frost1")
  local x, y, layer = hero:get_position()
  local prop = {x = x, y = y + 1, layer = layer, direction = 0, width = 16, height = 16,
    sprite = "things/ice_block"}
  local ice = map:create_custom_entity(prop)
  ice:set_drawn_in_y_order(true)
  -- Destroy ice block after a delay and hurt the hero.
  sol.timer.start(ice, ice_duration, function()
    ice:get_sprite():set_animation("breaking", function()
      ice:remove()
      hero:unfreeze()
      hero:start_hurt(frost_damage)
    end)
  end)
  sol.timer.start(ice, ice_duration - 200, function() sol.audio.play_sound("frost2") end)
  -- Remove iceball.
  self:remove()
end

-- Change damage.
function enemy:set_frost_damage(damage) frost_damage = damage end
function enemy:get_frost_damage() return frost_damage end

local enemy = ...

local shadow_sprite = nil
local initial_y = nil
local state = nil  -- "raising", "attacking" or "destroying".
local timer

function enemy:on_created()

  self:set_life(1)
  self:set_damage(4)
  self:create_sprite("enemies/evil_tile")
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_invincible()
  self:set_attack_consequence("sword", "custom")

  shadow_sprite = sol.sprite.create("entities/shadow")
  shadow_sprite:set_animation("big")
end

function enemy:on_restarted()

  local x, y = self:get_position()
  initial_y = y

  local m = sol.movement.create("path")
  m:set_path{2,2}
  m:set_speed(16)
  self:start_movement(m)
  timer = sol.timer.start(self, 2000, function() self:go_hero() end)
  state = "raising"
end

function enemy:go_hero()

  local angle = self:get_angle(self:get_map():get_entity("hero"))
  local m = sol.movement.create("straight")
  m:set_speed(192)
  m:set_angle(angle)
  m:set_smooth(false)
  self:start_movement(m)
  state = "attacking"
end

function enemy:on_obstacle_reached()
  self:disappear()
end

function enemy:on_custom_attack_received(attack, sprite)

  if state == "attacking" then
    self:disappear()
  end
end

function enemy:disappear()

  if state ~= "destroying" then
    state = "destroying"
    local sprite = self:get_sprite()
    self:set_can_attack(false)
    self:stop_movement()
    sprite:set_animation("destroy")
    sol.audio.play_sound("stone")
    if timer ~= nil then
      timer:stop()
    end
  end
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if state == "destroying" then
    self:remove()
  end
end

function enemy:on_pre_draw()

  -- Show the shadow.
  if state ~= "destroying" then
    local x, y = self:get_position()
    if state == "attacking" then
      y = y + 16
    else
      y = initial_y or y
    end
    self:get_map():draw_sprite(shadow_sprite, x, y)
  end
end


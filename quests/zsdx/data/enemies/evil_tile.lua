local enemy = ...

local shadow_sprite = nil
local initial_y = nil
local state = nil  -- "raising", "attacking" or "destroying".

function enemy:on_created()

  self:set_life(1)
  self:set_damage(4)

  local sprite = self:create_sprite("enemies/evil_tile")
  function sprite:on_animation_finished(animation)
    if state == "destroying" then
      enemy:remove()
    end
  end

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
  m:start(self)
  sol.timer.start(self, 2000, function() self:go_hero() end)
  state = "raising"
end

function enemy:go_hero()

  local angle = self:get_angle(self:get_map():get_entity("hero"))
  local m = sol.movement.create("straight")
  m:set_speed(192)
  m:set_angle(angle)
  m:set_smooth(false)
  m:start(self)
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
    self:set_attack_consequence("sword", "ignored")
    self:set_can_attack(false)
    self:stop_movement()
    sprite:set_animation("destroy")
    sol.audio.play_sound("stone")
    sol.timer.stop_all(self)
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


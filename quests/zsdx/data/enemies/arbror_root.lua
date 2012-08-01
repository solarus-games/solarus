local enemy = ...

-- A root of Master Arbror

local immobilized = false
local disappearing = false
local speed = 32
local timer
local father_name

function enemy:on_appear()

  self:set_life(1)
  self:set_damage(4)
  self:create_sprite("enemies/arbror_root")
  self:set_size(64, 16)
  self:set_origin(28, 6)
  self:set_invincible()
  self:set_attack_consequence("hookshot", "immobilized")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("sword", "protected")
  self:set_push_hero_on_sword(true)
  self:set_can_hurt_hero_running(true)
end

function enemy:on_restart()

  local sprite = self:get_sprite()
  if disappearing then
    sprite:set_animation("disappearing")
    self:set_invincible()
    self:stop_movement()
    self:set_can_attack(false)
  elseif immobilized then
    sprite:set_animation("hurt_long")
    if timer ~= nil then
      timer:stop()
    end
    timer = sol.timer.start(10000, function() self:disappear() end)
    self:stop_movement()
    self:set_can_attack(false)
  else
    timer = sol.timer.start(1000, function() self:go() end)
    self:set_can_attack(true)
  end
end

function enemy:go()

  if not immobilized then
    local m = sol.movement.movement_create("path_finding")
    m:set_speed(speed)
    self:start_movement(m)
  end
end

function enemy:on_hurt(attack, life_points)

  if not immobilized then
    -- tell my father that I will be immobilized
    father_name = self:get_father()
    if father_name ~= "" then
      self:send_message(father_name, "begin immobilized")
    end
    if timer ~= nil then
      timer:stop()
    end
  end
end

function enemy:on_immobilized()

  -- just immobilized
  immobilized = true
  self:restart() -- to stop the buit-in behavior of being immobilized
end

function disappear()

  if not disappearing then
    local sprite = self:get_sprite()
    sprite:set_animation("disappearing")
    self:set_invincible()
    self:set_can_attack(false)

    father_name = self:get_father()
    if father_name ~= "" then
      self:send_message(father_name, "end immobilized")
    end
    if timer ~= nil then
      timer:stop()
    end
    disappearing = true
  end
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if animation == "disappearing" then
    self:get_map():enemy_remove(self:get_name())
  end
end

function enemy:on_message_received(src_enemy, message)

  if message == "disappear" then
    self:disappear()
  else
    -- the message is the speed
    speed = tonumber(message)
  end
end


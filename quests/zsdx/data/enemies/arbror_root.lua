local enemy = ...

-- A root of Master Arbror

enemy.disappearing = false
enemy.timer = nil
enemy.immobilized = false
enemy.speed = 32
enemy.master_arbror = nil

function enemy:on_created()

  self:set_life(1)
  self:set_damage(4)
  self:set_size(64, 16)
  self:set_origin(28, 6)
  self:set_invincible()
  self:set_attack_consequence("hookshot", "immobilized")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("sword", "protected")
  self:set_push_hero_on_sword(true)
  self:set_can_hurt_hero_running(true)

  local sprite = self:create_sprite("enemies/arbror_root")
  function sprite:on_animation_finished(animation)
    if animation == "disappearing" then
      enemy:remove()
    end
  end
end

function enemy:on_restarted()

  local sprite = self:get_sprite()
  if self.disappearing then
    sprite:set_animation("disappearing")
    self:set_invincible()
    self:stop_movement()
    self:set_can_attack(false)
  elseif self.immobilized then
    sprite:set_animation("hurt_long")
    if self.timer ~= nil then
      self.timer:stop()
    end
    self.timer = sol.timer.start(self, 10000, function() self:disappear() end)
    self:stop_movement()
    self:set_can_attack(false)
  else
    self.timer = sol.timer.start(self, 1000, function() self:go() end)
    self:set_can_attack(true)
  end
end

function enemy:go()

  if not self.immobilized then
    local m = sol.movement.movement_create("path_finding")
    m:set_speed(self.speed)
    m:start(self)
  end
end

function enemy:on_hurt(attack, life_points)

  if not self.immobilized then
    -- Tell Master Arbror that I am immobilized.
    if self.master_arbror ~= nil then
      self.master_arbror:son_started_immobilized()
    end
    if self.timer ~= nil then
      self.timer:stop()
    end
  end
end

function enemy:on_immobilized()

  -- Just immobilized.
  self.immobilized = true
  self:restart()  -- To stop the buit-in behavior of being immobilized.
end

function enemy:disappear()

  if not self.disappearing then
    local sprite = self:get_sprite()
    sprite:set_animation("disappearing")
    self:set_invincible()
    self:set_can_attack(false)

    if self.timer ~= nil then
      self.timer:stop()
    end
    self.disappearing = true
  end
end


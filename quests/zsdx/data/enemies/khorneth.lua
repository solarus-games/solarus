local enemy = ...

-- The boss Khorneth from @PyroNet.
-- Khorneth has two blades that must be destroyed first.

local main_sprite = nil
local left_blade_sprite = nil
local right_blade_sprite = nil

-- State.
local left_blade_life = 4
local right_blade_life = 4
local blade_attack = false

function enemy:on_created()

  self:set_life(5)
  self:set_damage(2)
  self:set_pushed_back_when_hurt(false)
  main_sprite = self:create_sprite("enemies/khorneth")
  left_blade_sprite = self:create_sprite("enemies/khorneth_left_blade")
  right_blade_sprite = self:create_sprite("enemies/khorneth_right_blade")
  self:set_size(40, 48)
  self:set_origin(20, 25)
  self:set_invincible()
  self:set_attack_consequence_sprite(left_blade_sprite, "sword", "custom")
  self:set_attack_consequence_sprite(right_blade_sprite, "sword", "custom")

  -- When a blade sprite has the same animation than the
  -- main sprite, synchronize their frames
  left_blade_sprite:synchronize(main_sprite)
  right_blade_sprite:synchronize(main_sprite)
end

function enemy:on_restarted()

  local m = sol.movement.create("random_path")
  m:set_speed(48)
  self:start_movement(m)

  -- Schedule a blade attack
  if self:has_blade() then
    local delay = 1000 * (1 + math.random(4))
    sol.timer.start(self, delay, function() self:start_blade_attack() end)
    blade_attack = false
  end
end

function enemy:has_left_blade()
  return left_blade_life > 0
end

function enemy:has_right_blade()
  return right_blade_life > 0
end

function enemy:has_blade()
  return self:has_left_blade() or self:has_right_blade()
end

-- The enemy receives an attack whose consequence is "custom".
function enemy:on_custom_attack_received(attack, sprite)

  if self:has_left_blade()
    and sprite == left_blade_sprite
    and sprite:get_animation() ~= "stopped" then

    sprite:set_animation("hurt")
    main_sprite:set_animation("stopped")
    if self:has_right_blade() then
      right_blade_sprite:set_animation("stopped")
    end
    self:stop_movement()
    sol.audio.play_sound("boss_hurt")
    left_blade_life = left_blade_life - 1
    sol.timer.start(self, 400, function() self:stop_hurting_left_blade() end)

  elseif self:has_right_blade()
    and sprite == right_blade_sprite
    and sprite:get_animation() ~= "stopped" then

    sprite:set_animation("hurt")
    main_sprite:set_animation("stopped")
    if self:has_left_blade() then
      left_blade_sprite:set_animation("stopped")
    end
    self:stop_movement()
    sol.audio.play_sound("boss_hurt")
    right_blade_life = right_blade_life - 1
    sol.timer.start(self, 400, function() self:stop_hurting_right_blade() end)
  end
end

function enemy:start_blade_attack()

  if self:has_blade() and not blade_attack then

    blade_attack = true
    local side
    if not self:has_right_blade() then
      side = 0
    elseif not self:has_left_blade() then
      side = 1
    else
      side = math.random(2) - 1
    end

    if side == 0 then
      animation = "left_blade_attack"
    else
      animation = "right_blade_attack"
    end

    main_sprite:set_animation(animation)
    if self:has_left_blade() then
      left_blade_sprite:set_animation(animation)
    end
    if self:has_right_blade() then
      right_blade_sprite:set_animation(animation)
    end

    self:stop_movement()
  end
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if blade_attack and sprite == main_sprite then
    blade_attack = false
    self:restart()
  end
end

function enemy:stop_hurting_left_blade()

  self:restart()
  if left_blade_life <= 0 then
    sol.audio.play_sound("stone")
    self:remove_sprite(left_blade_sprite)

    if not self:has_right_blade() then
      self:start_final_phase()
    end
  end
end

function enemy:stop_hurting_right_blade()

  self:restart()
  if right_blade_life <= 0 then
    sol.audio.play_sound("stone")
    self:remove_sprite(right_blade_sprite)

    if not self:has_left_blade() then
      self:start_final_phase()
    end
  end
end

function enemy:start_final_phase()

  self:set_attack_consequence("sword", 1)
end


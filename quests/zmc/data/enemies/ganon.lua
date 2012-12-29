local enemy = ...

-- Ganon - final boss

-- Phase 1: Ganon periodically throws flames toward the hero.
-- 4 torches have to be light fast enough and in the clockwise order
-- (hint given by the childs) to make spawn a special item to throw to Ganon.
-- When touched by the item, Ganon is immobilized
-- and he becomes vulnerable to the sword.
-- While he is vulnerable, fire bats appear in circle to protect him.
-- After being immobilized, Ganon jumps towards the center and a
-- part of the floor is destroyed to make a lava hole.
-- Once four lava holes are created and Ganon is hurt again,
-- phase 2 starts.

-- Phase 2: Ganon also throws fire bats periodically.
-- The 4 torches now enable 4 solid switches on the sides.
-- Each switch may give a bonus among:
-- - special item to throw
-- - heart or magic flask
-- - kill all small enemies
-- - wrong switch, add more fire bats

local phase = 1
local vulnerable = false
local jumping = false
local attacking = false
local nb_floors_destroyed = 0
local nb_flames_created = 0
local nb_bats_created = 0
local attack_scheduled = false
local cancel_next_attack = false
local timers = {}

function enemy:on_created()

  self:set_life(1000000)
  self:set_damage(16)
  self:create_sprite("enemies/ganon")
  self:set_optimization_distance(0)
  self:set_size(32, 32)
  self:set_origin(16, 29)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("thrown_item", "immobilized")
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
end

function enemy:on_restarted()

  if not jumping and not attacking then
    if not vulnerable then
      m = sol.movement.create("path_finding")
      m:set_speed(64)
      self:start_movement(m)
      self:set_hurt_style("normal")
      if not attack_scheduled then
	self:schedule_attack()
      end
    else
      self:jump()
    end
  end
end

function enemy:on_movement_changed(movement)

  -- Take the appropriate sprite direction.
  local direction4 = movement:get_direction4()
  local sprite = self:get_sprite()
  if direction4 == 1 then
    sprite:set_direction(1)
  else
    sprite:set_direction(0)
  end
end

function enemy:on_immobilized()

  if not vulnerable then
    vulnerable = true
    self:set_attack_consequence("sword", 1)
    self:set_attack_consequence("thrown_item", "protected")
    self:set_hurt_style("boss")

    -- Make a protection.
    if self:get_map():get_entities_count(self:get_name() .. "_bats_") < 9 then
      attacking = false
      self:throw_bats()
      cancel_next_attack = true  -- Otherwise two attacks would be scheduled.
    end
  end
end

function enemy:jump()

  local angle = math.pi + self:get_angle(self:get_map():get_entity("hero"))
  local m = sol.movement.create("target")
  m:set_speed(128)
  m:set_target(240, 245)
  self:start_movement(m)
  sol.audio.play_sound("jump")
  local sprite = self:get_sprite()
  sprite:set_animation("jumping")
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("thrown_item", "ignored")
  vulnerable = false
  jumping = true
  self:set_can_attack(false)
end

function enemy:on_obstacle_reached(movement)

  self:on_movement_finished(movement)
end

function enemy:on_movement_finished(movement)

  local sprite = self:get_sprite()
  if sprite:get_animation() == "jumping" then
    self:finish_jump()
  end
end

function enemy:finish_jump()

  if phase == 1 then
    -- Destroy floors.
    local floors = { "floor_left_", "floor_right_", "floor_up_", "floor_down_" }
    nb_floors_destroyed = nb_floors_destroyed + 1
    if nb_floors_destroyed <= #floors then
      self:destroy_floor(floors[nb_floors_destroyed], 1, 50)
    else
      -- Go to phase 2.
      self:set_life(24)
      phase = 2
    end
  end

  jumping = false
  self:set_attack_consequence("thrown_item", "immobilized")
  self:set_can_attack(true)
  self:restart()
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if animation == "jumping" then
    self:finish_jump()
  end
end

function enemy:destroy_floor(prefix, first, last)

  local index = first
  local delay = 30

  function repeat_change()
    if index % 10 == 1 then
      sol.audio.play_sound("stone")
    end

    self:get_map():entities_set_enabled(prefix .. index, false)

    if index ~= last then
      sol.timer.start(self, delay, repeat_change)
    end
    index = index + 1
  end
  repeat_change()
end

function enemy:attack()

  if phase == 1 or math.random(2) == 1 then
    self:throw_flames()
  else
    self:throw_bats()
  end
end

function enemy:throw_flames()

  if vulnerable or jumping or attacking then
    return
  end

  if cancel_next_attack then
    cancel_next_attack = false
    return
  end

  attacking = true

  local prefix = self:get_name() .. "_flame_"
  local nb_to_create = (1 + nb_floors_destroyed) * 5

  function repeat_throw_flame()

    if vulnerable then
      -- Got immobilized while shooting flames.
      attacking = false
      return
    end

    sol.audio.play_sound("lamp")
    nb_flames_created = nb_flames_created + 1
    local son_name = prefix .. nb_flames_created
    self:create_enemy(son_name, "red_flame", 0, -24, 0)
    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      timers[#timers + 1] = sol.timer.start(self, 150, repeat_throw_flame)
    else
      attacking = false
      attack_scheduled = false
      self:restart()
    end
  end
  self:stop_movement()
  repeat_throw_flame()
end

function enemy:throw_bats()

  if jumping or attacking then
    return
  end

  if cancel_next_attack then
    cancel_next_attack = false
    return
  end

  attacking = true

  local prefix = self:get_name() .. "_bat_"
  local nb_to_create = 9

  function repeat_throw_bat()

    sol.audio.play_sound("lamp")
    nb_bats_created = nb_bats_created + 1
    local son_name = prefix .. nb_bats_created
    local son = self:create_enemy(son_name, "fire_bat", 0, -21, 0)
    if math.random(6) == 1 then
      son:set_treasure("magic_flask", 1, -1)
    end
    son:go_circle(self)
    local go_hero_delay = 2000 + (nb_to_create * 150)
    timers[#timers + 1] = sol.timer.start(self, go_hero_delay, function() son:go_hero() end)

    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      timers[#timers + 1] = sol.timer.start(self, 233, repeat_throw_bat)
    else
      attacking = false
      attack_scheduled = false
      if not vulnerable then
	self:restart()
      else
	cancel_next_attack = false
      end
    end
  end
  self:stop_movement()
  repeat_throw_bat()
end

function enemy:schedule_attack()

  timers[#timers + 1] = sol.timer.start(self, math.random(3000, 6000), function()
    self:attack()
  end)
  attack_scheduled = true
end

function enemy:on_hurt(attack, life_lost)

  if self:get_life() <= 0 then
    for _, t in ipairs(timers) do t:stop() end
  end
end


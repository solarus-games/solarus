local enemy = ...

-- Poutroxor: an evil giant bird from Newlink.
-- Phase 1: a skeleton is on the bird and has to be killed with arrows.
-- Phase 2: once the skeleton is dead, the bird moves faster, throws blue flames and can be killed by shooting arrows in his head.

local bird, head, skeleton  -- Sprites.
local phase = 1
local nb_flames_created = 0
local max_flames_created = 10
local timers = {}

function enemy:on_created()

  self:set_life(14)
  self:set_damage(1)
  bird = self:create_sprite("enemies/poutroxor")
  head = self:create_sprite("enemies/poutroxor_head")
  skeleton = self:create_sprite("enemies/poutroxor_skeleton")
  self:set_size(16, 16)
  self:set_origin(8, 8)
  self:set_obstacle_behavior("flying")
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)

  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("arrow", "ignored")
  self:set_attack_consequence_sprite(skeleton, "arrow", 1)
end

function enemy:on_restarted()

  if phase == 1 then
    local m = sol.movement.create("random")
    m:set_speed(32)
    self:start_movement(m)
    for _, t in ipairs(timers) do t:stop() end
    timers[#timers + 1] = sol.timer.start(self, math.random(2000, 3000), function()
      self:skeleton_attack()
    end)
  else
    local m = sol.movement.create("random")
    m:set_speed(80)
    self:start_movement(m)
    for _, t in ipairs(timers) do t:stop() end
    timers[#timers + 1] = sol.timer.start(self, math.random(3000, 5000), function()
      self:big_attack()
    end)
  end
end

-- Make the skeleton shoot a blue flame.
function enemy:skeleton_attack()

  skeleton:set_animation("attack")
  sol.audio.play_sound("ice")
  timers[#timers + 1] = sol.timer.start(self, 500, function()
    skeleton:set_animation("walking")
    nb_flames_created = nb_flames_created + 1
    local son_name = self:get_name() .. "_son_" .. nb_flames_created
    local son = self:create_enemy(son_name, "blue_flame", 0, -48, 2)
    local angle = son:get_angle(self:get_map():get_entity("hero"))
    son:go(angle)
    timers[#timers + 1] = sol.timer.start(self, math.random(1000, 3000), function()
      self:skeleton_attack()
    end)
  end)
end

function enemy:on_hurt(attack, life_lost)

  for _, t in ipairs(timers) do t:stop() end
  if phase == 1 and self:get_life() <= 7 then
    self:stop_movement()
    sol.audio.play_sound("enemy_killed")
    skeleton:set_animation("hurt")
    phase = 2
    self:remove_sprite(skeleton)
    self:get_map():remove_entities(self:get_name() .. "_son")
    skeleton = nil
    self:set_attack_consequence_sprite(head, "arrow", 1)
  elseif self:get_life() <= 0 then
    self:get_map():remove_entities(self:get_name() .. "_son")
  end
end

-- Makes the bird throw several blue flames.
function enemy:big_attack()

  nb_flames_created = 0
  self:stop_movement()
  head:set_animation("attack")
  sol.audio.play_sound("lamp")
  timers[#timers + 1] = sol.timer.start(self, 500, function() self:repeat_flame() end)
end

function enemy:repeat_flame()

  if nb_flames_created <= max_flames_created then
    local son_name = self:get_name() .. "_son_" .. nb_flames_created
    local angle = math.random(30, 90) * math.pi / 40
    nb_flames_created = nb_flames_created + 1
    local son = self:create_enemy(son_name, "blue_flame", 0, 16)
    son:go(angle)
    sol.audio.play_sound("lamp")
    timers[#timers + 1] = sol.timer.start(self, 150, function() self:repeat_flame() end)
  else
    timers[#timers + 1] = sol.timer.start(self, 500, function() self:restart() end)
  end
end


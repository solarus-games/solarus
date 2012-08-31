local enemy = ...

-- Drakomos head.

local max_fire_created = 10   -- Maximum for a step.
local nb_fire_created = 0     -- In the current step.
local total_fire_created = 0  -- Total on all steps.
local initial_xy = {}
local timers = {}

function enemy:on_created()

  self:set_life(16)
  self:set_damage(16)
  self:create_sprite("enemies/drakomos_head")
  self:set_size(56, 56)
  self:set_origin(28, 40)
  self:set_hurt_style("boss")
  self:set_obstacle_behavior("flying")
  self:set_no_treasure()

  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("thrown_item", 1)

  initial_xy.x, initial_xy.y = self:get_position()
end

function enemy:on_restarted()

  local m = sol.movement.create("target")
  m:set_speed(48)
  m:set_target(initial_xy.x, initial_xy.y)
  self:start_movement(m)
  for _, t in ipairs(timers) do t:stop() end

  nb_fire_created = 0
  timers[#timers + 1] = sol.timer.start(self, 2000 + math.random(8000), function()
    self:stop_movement()
    local sprite = self:get_sprite()
    sprite:set_animation("preparing_fire")
    sol.audio.play_sound("lamp")
    timers[#timers + 1] = sol.timer.start(self, 500, function() self:repeat_fire() end)
  end)
end

function enemy:on_hurt(attack, life_lost)

  if life_lost > 0 then
    for _, t in ipairs(timers) do t:stop() end
  end
end

function enemy:on_movement_finished(movement)

  local m = sol.movement.create("random")
  m:set_speed(48)
  m:set_max_distance(24)
  self:start_movement(m)
end

function enemy:repeat_fire()

  if nb_fire_created < max_fire_created then
    nb_fire_created = nb_fire_created + 1
    total_fire_created = total_fire_created + 1
    local son_name = self:get_name() .. "_son_" .. total_fire_created
    local angle_start = 2 * math.pi / 4
    local angle_end = 9 * math.pi / 4
    local angle = angle_start + nb_fire_created * (angle_end - angle_start) / max_fire_created
    local son = self:create_enemy(son_name, "fireball_simple", 0, 16)
    son:go(angle)
    sol.audio.play_sound("lamp")
    timers[#timers + 1] = sol.timer.start(self, 150, function() self:repeat_fire() end)
  else
    timers[#timers + 1] = sol.timer.start(self, 500, function() self:restart() end)
  end
end


local enemy = ...

-- A big butterfly boss from Newlink.

local nb_eggs_to_create = 0
local nb_eggs_created = 0
local boss_starting_life = 6
local boss_movement_starting_speed = 50  -- Starting speed in pixels per second, it will gain 5 per life point lost.
local boss_movement_speed = boss_movement_starting_speed
local timers = {}

function enemy:on_created()

  self:set_life(boss_starting_life)
  self:set_damage(2)
  self:create_sprite("enemies/papillosaur_king")
  self:set_size(176, 96)
  self:set_origin(88, 64)
  self:set_invincible()
  self:set_attack_consequence("explosion", 1)
  self:set_attack_consequence("sword", "protected")
  self:set_obstacle_behavior("flying")
end

function enemy:on_restarted()

  for _, t in ipairs(timers) do t:stop() end
  timers[#timers + 1] = sol.timer.start(self, 2000, function() self:egg_phase_soon() end)
  self:go()
end

function enemy:on_hurt(attack, life_lost)

  for _, t in ipairs(timers) do t:stop() end
  local life = self:get_life()
  if life <= 0 then
    -- I am dying: remove the minillosaur eggs.
    local sons_prefix = self:get_name() .. "_minillosaur"
    self:get_map():remove_entities(sons_prefix)
  else
    boss_movement_speed = boss_movement_starting_speed
      + (boss_starting_life - life) * 5
  end
end

function enemy:go()
  local m
  if self:get_life() > 1 then
    m = sol.movement.create("random_path")
    m:set_speed(boss_movement_speed)
  else
    -- The enemy is now desperate and angry against our hero.
    m = sol.movement.create("target")
    m:set_speed(boss_movement_speed)
  end
  m:start(self)
end

function enemy:egg_phase_soon()

  local sons_prefix = self:get_name() .. "_minillosaur"
  local nb_sons = self:get_map():get_entities_count(sons_prefix)
  if nb_sons >= 5 then
    -- Delay the egg phase if there are already too much sons.
    timers[#timers + 1] = sol.timer.start(self, 5000, function() self:egg_phase_soon() end)
  else
    self:stop_movement()
    timers[#timers + 1] = sol.timer.start(self, 500, function() self:egg_phase() end)
  end
end

function enemy:egg_phase()

  local sprite = self:get_sprite()
  sprite:set_animation("preparing_egg")
  sol.audio.play_sound("boss_charge")
  timers[#timers + 1] = sol.timer.start(self, 1500, function() self:throw_egg() end)

  -- The more the boss is hurt, the more it will throw eggs...
  nb_eggs_to_create = boss_starting_life - self:get_life() + 1
end

function enemy:throw_egg()

  -- Create the egg.
  nb_eggs_created = nb_eggs_created + 1
  local egg_name = self:get_name() .. "_minillosaur_" .. nb_eggs_created
  local egg = self:create_enemy(egg_name, "minillosaur_egg_thrown", 0, 16)
  egg:set_no_treasure()
  sol.audio.play_sound("boss_fireball")

  -- See what to do next.
  nb_eggs_to_create = nb_eggs_to_create - 1
  if nb_eggs_to_create > 0 then
    -- Throw another egg in 0.5 second.
    timers[#timers + 1] = sol.timer.start(self, 500, function() self:throw_egg() end)
  else
    -- Finish the egg phase.
    local sprite = self:get_sprite()
    sprite:set_animation("walking")
    -- Don't throw eggs when desperate!
    if self:get_life() > 1 then
      -- Schedule the next one in a few seconds.
      local delay = 3500 + (math.random(3) * 1000)
      timers[#timers + 1] = sol.timer.start(self, duration, function() self:egg_phase_soon() end)
    end
    self:go()
  end
end


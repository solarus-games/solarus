local enemy = ...

-- A big butterfly boss from Newlink.

local nb_eggs_to_create = 0
local nb_eggs_created = 0
local boss_starting_hp = 6
local boss_movement_starting_speed = 50 -- Starting speed in pixels per second, it will gain 5 per hp lost.
local boss_movement_speed = boss_movement_starting_speed
local timers = {}

function enemy:on_appear()

  self:set_life(boss_starting_hp)
  self:set_damage(2)
  self:create_sprite("enemies/papillosaur_king")
  self:set_size(176, 96)
  self:set_origin(88, 64)
  self:set_invincible()
  self:set_attack_consequence("explosion", 1)
  self:set_attack_consequence("sword", "protected")
  self:set_obstacle_behavior("flying")
end

function enemy:on_restart()

  for _, t in ipairs(timers) do t:stop() end
  timers[#timers + 1] = sol.timer.start(2000, egg_phase_soon)
  go()
end

function enemy:on_hurt(attack, life_lost)

  for _, t in ipairs(timers) do t:stop() end
  local boss_hp = self:get_life()
  if boss_hp <= 0 then
    -- I am dying: remove the minillosaur eggs
    local sons_prefix = self:get_name().."_minillosaur"
    self:get_map():enemy_remove_group(sons_prefix)
    -- TODO self:get_map():enemy_remove_sons or even self:get_map():enemy_kill_sons
  else
    boss_movement_speed = boss_movement_starting_speed
      + (boss_starting_hp - boss_hp) * 5
  end
end

function go()
  local m
  if self:get_life() > 1 then
    m = sol.movement.create("random_path")
    m:set_speed(boss_movement_speed)
  else
    -- The enemy is now desperate and angry against our hero
    m = sol.movement.create("target")
    m:set_speed(boss_movement_speed)
  end
  self:start_movement(m)
end

function egg_phase_soon()

  local sons_prefix = self:get_name().."_minillosaur"
  local nb_sons = self:get_map():enemy_get_group_count(sons_prefix)
  if nb_sons >= 5 then
    -- delay the egg phase if there are already too much sons
    timers[#timers + 1] = sol.timer.start(5000, egg_phase_soon)
  else
    self:stop_movement()
    timers[#timers + 1] = sol.timer.start(500, egg_phase)
  end
end

function egg_phase()

  local sprite = self:get_sprite()
  sprite:set_animation("preparing_egg")
  sol.audio.play_sound("boss_charge")
  timers[#timers + 1] = sol.timer.start(1500, throw_egg)

  -- The more the boss is hurt, the more it will throw eggs...
  nb_eggs_to_create = boss_starting_hp - self:get_life() + 1

end

function throw_egg()

  -- create the egg
  nb_eggs_created = nb_eggs_created + 1
  local egg_name = self:get_name().."_minillosaur_"..nb_eggs_created
  self:create_son(egg_name, "minillosaur_egg_thrown", 0, 16)
  self:get_map():enemy_set_treasure(egg_name, "_none", 1, -1)
  sol.audio.play_sound("boss_fireball")

  -- see what to do next
  nb_eggs_to_create = nb_eggs_to_create - 1
  if nb_eggs_to_create > 0 then
    -- throw another egg in 0.5 second
    timers[#timers + 1] = sol.timer.start(500, throw_egg)
  else
    -- finish the egg phase
    local sprite = self:get_sprite()
    sprite:set_animation("walking")
    -- don't throw eggs when desperate!
    if self:get_life() > 1 then
      -- schedule the next one in a few seconds
      local delay = 3500 + (math.random(3) * 1000)
      timers[#timers + 1] = sol.timer.start(duration, egg_phase_soon)
    end
    go()
  end
end


local enemy = ...

-- Agahnim (Boss of dungeon 5)

-- Possible positions where he appears.
local positions = {
  {x = 192, y = 277, direction4 = 3},
  {x = 400, y = 277, direction4 = 3},
  {x = 296, y = 373, direction4 = 1}
}

local nb_sons_created = 0
local initial_life = 10
local finished = false
local blue_fireball_proba = 33  -- Percent.
local vulnerable = false
local timers = {}

function enemy:on_created()

  self:set_life(initial_life)
  self:set_damage(12)
  self:create_sprite("enemies/agahnim")
  self:set_optimization_distance(0)
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)

  local sprite = self:get_sprite()
  sprite:set_animation("stopped")
end

function enemy:on_restarted()

  vulnerable = false
  for _, t in ipairs(timers) do t:stop() end
  local sprite = self:get_sprite()

  if not finished then
    sprite:fade_out()
    timers[#timers + 1] = sol.timer.start(self, 700, function() self:hide() end)
  else
    sprite:set_animation("hurt")
    self:get_map():hero_freeze()
    timers[#timers + 1] = sol.timer.start(self, 500, function() self:end_dialog() end)
    timers[#timers + 1] = sol.timer.start(self, 1000, function() self:fade_out() end)
    timers[#timers + 1] = sol.timer.start(self, 1500, function() self:escape() end)
  end
end

function enemy:hide()

  vulnerable = false
  self:set_position(-100, -100)
  timers[#timers + 1] = sol.timer.start(self, 500, function() self:unhide() end)
end

function enemy:unhide()

  local position = (positions[math.random(#positions)])
  self:set_position(position.x, position.y)
  local sprite = self:get_sprite()
  sprite:set_direction(position.direction4)
  sprite:fade_in()
  timers[#timers + 1] = sol.timer.start(self, 1000, function() self:fire_step_1() end)
end

function enemy:fire_step_1()

  local sprite = self:get_sprite()
  sprite:set_animation("arms_up")
  timers[#timers + 1] = sol.timer.start(self, 1000, function() self:fire_step_2() end)
end

function enemy:fire_step_2()

  local sprite = self:get_sprite()
  if math.random(100) <= blue_fireball_proba then
    sprite:set_animation("preparing_blue_fireball")
  else
    sprite:set_animation("preparing_red_fireball")
  end
  sol.audio.play_sound("boss_charge")
  timers[#timers + 1] = sol.timer.start(self, 1500, function() self:fire_step_3() end)
end

function enemy:fire_step_3()

  local sprite = self:get_sprite()

  local sound, breed
  if sprite:get_animation() == "preparing_blue_fireball" then
    sound = "cane"
    breed = "blue_fireball_triple"
  else
    sound = "boss_fireball"
    breed = "red_fireball_triple"
  end
  sprite:set_animation("stopped")
  sol.audio.play_sound(sound)

  vulnerable = true
  timers[#timers + 1] = sol.timer.start(self, 700, function() self:restart() end)

  function throw_fire()

    nb_sons_created = nb_sons_created + 1
    self:create_enemy("agahnim_fireball_" .. nb_sons_created, breed, 0, -21)
  end

  throw_fire()
  if self:get_life() <= initial_life / 2 then
    timers[#timers + 1] = sol.timer.start(self, 200, function() self:throw_fire() end)
    timers[#timers + 1] = sol.timer.start(self, 400, function() self:throw_fire() end)
  end
end

function enemy:receive_bounced_fireball(fireball)

  if fireball:get_name():find("^agahnim_fireball")
      and vulnerable then
    -- Receive a fireball shot back by the hero: get hurt.
    for _, t in ipairs(timers) do t:stop() end
    fireball:remove()
    self:hurt(1)
  end
end

function enemy:on_hurt(attack, life_lost)

  local life = self:get_life()
  if life <= 0 then
    self:get_map():remove_entities("agahnim_fireball")
    self:set_life(1)
    finished = true
  elseif life <= initial_life / 3 then
    blue_fireball_proba = 50
  end
end

function enemy:end_dialog()

  self:get_map():remove_entities("agahnim_fire_ball_" .. i)
  local sprite = self:get_sprite()
  sprite:set_ignore_suspend(true)
  self:get_map():start_dialog("dungeon_5.agahnim_end")
end

function enemy:fade_out()

  local sprite = self:get_sprite()
  sprite:fade_out()
end

function enemy:escape()

  local x, y = self:get_position()
  self:get_map():create_pickable{
    treasure_name = "heart_container",
    treasure_variant = 1,
    treasure_savegame_variable = 521,
    x = x,
    y = y,
    layer = 0
  }
  self:get_map():get_entity("hero"):unfreeze()
  self:get_game():set_boolean(520, true)
  self:remove()
end


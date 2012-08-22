local enemy = ...

-- Agahnim (Boss of dungeon 8)

-- possible positions where he appears
local positions = {
  {x = 488, y = 317},
  {x = 560, y = 317},
  {x = 704, y = 317},
  {x = 776, y = 317},
  {x = 488, y = 421},
  {x = 560, y = 421},
  {x = 704, y = 421},
  {x = 776, y = 421},
  {x = 488, y = 525},
  {x = 560, y = 525},
  {x = 704, y = 525},
  {x = 776, y = 525},
}

local nb_sons_created = 0
local initial_life = 16
local blue_fireball_proba = 33 -- percent
local next_fireball_sound
local next_fireball_breed
local vulnerable = false
local hurt_proba
local middle_dialog = false
local nb_fakes_created = 0
local timers = {}

function enemy:on_created()

  self:set_life(initial_life)
  self:set_damage(16)
  self:create_sprite("enemies/agahnim_2")
  self:set_optimization_distance(0)
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_position(-100, -100)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
  self:set_can_attack(false)

  local sprite = self:get_sprite()
  sprite:set_animation("stopped")
end

function enemy:on_restarted()

  vulnerable = false
  for _, t in ipairs(timers) do t:stop() end
  local sprite = self:get_sprite()
  sprite:set_ignore_suspend(false)
  sprite:fade_out()
  timers[#timers + 1] = sol.timer.start(700, function()
    self:hide()
  end)
end

function enemy:on_update()

  -- look in the direction of the hero
  local sprite = self:get_sprite()
  sprite:set_direction(self:get_direction4_to_hero())
end

function enemy:get_direction4_to_hero()

  local x, y = self:get_position()
  local hero_x, hero_y = self:get_map():hero_get_position()
  local angle = sol.main.get_angle(x, y, hero_x, hero_y)
  local direction4 = (angle + (math.pi / 4)) * 2 / math.pi
  return (math.floor(direction4) + 4) % 4
end

function enemy:hide()

  -- disappear for a while
  vulnerable = false
  self:set_position(-100, -100)
  timers[#timers + 1] = sol.timer.start(500, function()
    self:unhide()
  end)
end

function enemy:unhide()

  -- come back somewhere
  local position = (positions[math.random(#positions)])
  self:set_position(position.x, position.y)
  local sprite = self:get_sprite()
  sprite:set_direction(self:get_direction4_to_hero())
  sprite:fade_in()
  timers[#timers + 1] = sol.timer.start(1000, function()
    self:fire_step_1()
  end)
end

function enemy:fire_step_1()

  -- before preparing a fireball
  local sprite = self:get_sprite()
  sprite:set_animation("arms_up")
  timers[#timers + 1] = sol.timer.start(1000, function()
    self:fire_step_2()
  end)
  self:set_can_attack(true)
end

function enemy:fire_step_2()

  -- prepare a fireball (red or blue)
  local sprite = self:get_sprite()
  local blue = math.random(100) <= blue_fireball_proba

  if math.random(5) == 1 then
    -- don't tell the player if it will be red or blue
    sprite:set_animation("preparing_unknown_fireball")
  elseif blue then
    -- blue fireball: for the hero can do nothing but run away
    sprite:set_animation("preparing_blue_fireball")
  else
    -- red fireball: possible to throw it back to Agahnim
    sprite:set_animation("preparing_red_fireball")
  end

  if blue then
    next_fireball_sound = "cane"
    next_fireball_breed = "blue_fireball_triple"
  else
    next_fireball_sound = "boss_fireball"
    next_fireball_breed = "red_fireball_triple"
  end
  sol.audio.play_sound("boss_charge")
  timers[#timers + 1] = sol.timer.start(1500, function()
    self:fire_step_3()
  end)
end

function fire_step_3()

  -- throw the fireball(s)
  local sprite = self:get_sprite()
  sprite:set_animation("stopped")
  sol.audio.play_sound(next_fireball_sound)
  vulnerable = true

  local delay -- delay before fading out and going somewhere else
  if next_fireball_breed == "blue_fireball_triple" then
    delay = 700
  else
    delay = 3000 -- red fireball: stay longer to play ping-pong
  end
  timers[#timers + 1] = sol.timer.start(delay, function()
    self:restart()
  end)

  function throw_fire()
    nb_sons_created = nb_sons_created + 1
    self:create_son("agahnim_fireball_" .. nb_sons_created,
        next_fireball_breed, 0, -21)
  end

  self:throw_fire()

  -- shoot more fireballs if the life becomes short
  local life = self:get_life()
  if life <= initial_life / 2 then
    timers[#timers + 1] = sol.timer.start(200, function() self:throw_fire() end)
    timers[#timers + 1] = sol.timer.start(400, function() self:throw_fire() end)
    if life <= initial_life / 4 then
      timers[#timers + 1] = sol.timer.start(600, function() self:throw_fire() end)
      timers[#timers + 1] = sol.timer.start(800, function() self:throw_fire() end)
    end
  end

  -- play ping pong
  if life <= initial_life * 3 / 4 then
    hurt_proba = 20
  else
    hurt_proba = 100
  end
end

function enemy:on_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball")
      and vulnerable then

    -- receive a fireball: get hurt or throw it back
    if math.random(100) <= hurt_proba then
      for _, t in ipairs(timers) do t:stop() end
      self:get_map():enemy_remove(src_enemy)
      self:hurt(1)
    else
      -- play ping-pong
      sol.audio.play_sound("boss_fireball")
      self:send_message(src_enemy, "bounce")
      hurt_proba = hurt_proba + 20
    end
  end
end

function enemy:on_hurt(attack, life_lost)

  local sprite = self:get_sprite()
  local life = self:get_life()
  if life <= 0 then
    -- dying
    self:get_map():enemy_remove_group("agahnim_fireball")
    self:get_map():enemy_remove_group(self:get_name() .. "_")
    sprite:set_ignore_suspend(true)
    self:get_map():start_dialog("dungeon_8.agahnim_end")
    for _, t in ipairs(timers) do t:stop() end
  elseif life <= initial_life * 2 / 3 then
    -- not dying yet: start creating fakes after a few hits
    sprite:set_ignore_suspend(true)
    if not middle_dialog then
      self:get_map():start_dialog("dungeon_8.agahnim_middle")
      middle_dialog = true
    end
    self:create_fakes()
  end
end

-- create fake Agahnims
function enemy:create_fakes()

  local prefix = self:get_name() .. "_fake_"
  if self:get_map():enemy_get_group_count(prefix) < 3 then
    nb_fakes_created = nb_fakes_created + 1
    local fake_name = prefix .. nb_fakes_created
    self:create_son(fake_name, "agahnim_2_fake", 0, 0)
  end

  if self:get_life() < initial_life / 3
      and self:get_map():enemy_get_group_count(prefix) < 2 then
    -- create a second one
    self:create_fakes()
  end
end


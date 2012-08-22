local enemy = ...

-- Fake Agahnim (with the boss of dungeon 8)

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
local blue_fireball_proba = 33 -- percent
local next_fireball_sound
local next_fireball_breed
local disappearing = false
local timers = {}

function enemy:on_created()

  self:set_life(1)
  self:set_damage(8)
  self:create_sprite("enemies/agahnim_2_fake")
  self:set_optimization_distance(0)
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_position(-100, -100)
  self:set_invincible()
  self:set_attack_consequence("sword", "custom")
  self:set_attack_consequence("arrow", "custom")
  self:set_attack_consequence("hookshot", "custom")
  self:set_attack_consequence("boomerang", "custom")
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
  self:set_can_attack(false)

  local sprite = self:get_sprite()
  sprite:set_animation("stopped")

end

function enemy:on_restart()

  if not disappearing then
    for _, t in ipairs(timers) do t:stop() end
    local sprite = self:get_sprite()
    sprite:fade_out()
    timers[#timers + 1] = sol.timer.start(500, function()
      self:hide()
    end)
  end
end

function enemy:on_update()

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

  self:set_position(-100, -100)
  timers[#timers + 1] = sol.timer.start(500, function()
    self:unhide()
  end)
end

function enemy:unhide()

  local position = (positions[math.random(#positions)])
  self:set_position(position.x, position.y)
  local sprite = self:get_sprite()
  sprite:set_direction(get_direction4_to_hero())
  sprite:fade_in()
  timers[#timers + 1] = sol.timer.start(1000, function()
    self:fire_step_1()
  end)
end

function enemy:fire_step_1()

  local sprite = self:get_sprite()
  sprite:set_animation("arms_up")
  timers[#timers + 1] = sol.timer.start(1000, function()
    self:fire_step_2()
  end)
  self:set_can_attack(true)
end

function enemy:fire_step_2()

  local sprite = self:get_sprite()
  local blue = math.random(100) <= blue_fireball_proba

  if math.random(5) == 1 then
    sprite:set_animation("preparing_unknown_fireball")
  elseif blue then
    sprite:set_animation("preparing_blue_fireball")
  else
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

function enemy:fire_step_3()

  local sprite = self:get_sprite()
  sprite:set_animation("stopped")
  sol.audio.play_sound(next_fireball_sound)
  timers[#timers + 1] = sol.timer.start(700, function()
    self:restart()
  end)

  function throw_fire()

    nb_sons_created = nb_sons_created + 1
    self:create_son(self:get_name() .. "_fireball_" .. nb_sons_created,
        next_fireball_breed, 0, -21)
  end

  throw_fire()
  local life = self:get_life()
end

function enemy:on_custom_attack_received(attack, sprite)

  sol.audio.play_sound("enemy_hurt")
  self:disappear()
end

function enemy:disappear()

  local sprite = self:get_sprite()
  disappearing = true
  self:set_can_attack(false)
  sprite:fade_out()
  for _, t in ipairs(timers) do t:stop() end
  timers[#timers + 1] = sol.timer.start(500, function()
    self:get_map():enemy_remove(self:get_name())
  end)
end

function enemy:on_collision_enemy(other_name, other_sprite, my_sprite)

  if not other_name:find("fireball") then

    local x = self:get_position()
    if x > 0 then
      -- collision with another Agahnim
      for _, t in ipairs(timers) do t:stop() end
      self:hide() -- go somewhere else
    end
  end
end

function enemy:on_message_received(src_enemy, message)

  if string.find(src_enemy, "^agahnim_fireball") then
    -- receive a fireball: disappear
    self:disappear()
  end
end


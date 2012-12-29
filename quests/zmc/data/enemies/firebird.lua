local enemy = ...

-- Firebird

local main_sprite = nil
local claw_sprite = nil
local nb_sons_created = 0
local initial_life = 7
local timers = {}

function enemy:on_created()

  self:set_life(initial_life)
  self:set_damage(2)
  main_sprite = self:create_sprite("enemies/firebird")
  claw_sprite = self:create_sprite("enemies/firebird")
  claw_sprite:set_animation("claw")
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_obstacle_behavior("flying")
  self:set_layer_independent_collisions(true)

  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence_sprite(claw_sprite, "arrow", 1)
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
end

function enemy:on_restarted()

  claw_sprite:set_animation("claw")
  local m = sol.movement.create("random")
  m:set_speed(64)
  self:start_movement(m)
  for _, t in ipairs(timers) do t:stop() end
  timers[#timers + 1] = sol.timer.start(self, math.random(2000, 6000), function()
    self:prepare_flames()
  end)
end

function enemy:prepare_flames()

  local prefix = self:get_name() .. "_son_"
  nb_sons_created = nb_sons_created + 1
  local life_lost = initial_life - self:get_life()
  local nb_to_create = 3 + life_lost

  function repeat_throw_flame()
    sol.audio.play_sound("lamp")
    local son_name = prefix .. nb_sons_created
    self:create_enemy(son_name, "red_flame", 0, -16, 0)
    nb_to_create = nb_to_create - 1
    if nb_to_create > 0 then
      timers[#timers + 1] = sol.timer.start(self, 200, repeat_throw_flame)
    end
  end
  repeat_throw_flame()

  timers[#timers + 1] = sol.timer.start(self, math.random(4000, 6000), function()
    self:prepare_flames()
  end)
end

function enemy:on_hurt(attack, life_lost)

  for _, t in ipairs(timers) do t:stop() end
  if self:get_life() <= 0 then
    self:get_map():remove_entities(self:get_name() .. "_")
  end
end


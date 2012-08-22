local enemy = ...

-- A giant tree boss from Newlink

local nb_sons_created = 0
local nb_sons_immobilized = 0
local nb_sons_immobilized_needed = 3 -- number of sons immobilized needed to get him vulnerable
local vulnerable = false
local initial_life = 8
local timers = {}

function enemy:on_created()

  self:set_life(initial_life)
  self:set_damage(4)
  self:create_sprite("enemies/master_arbror")
  self:set_optimization_distance(0)
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_pushed_back_when_hurt(false)
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_push_hero_on_sword(true)
  self:set_can_hurt_hero_running(true)
end

function enemy:on_restarted()

  local sprite = self:get_sprite()
  if not vulnerable then
    self:go()
  else
    sprite:set_animation("vulnerable")
  end
end

function enemy:go()

  local m = sol.movement.create("random")
  m:set_speed(16)
  m:set_max_distance(16)
  self:start_movement(m)
  for _, t in ipairs(timers) do t:stop() end
  timers[#timers + 1] = sol.timer.start(math.random(2000, 3000), function()
    self:prepare_son()
  end)
end

function enemy:on_hurt(attack, life_lost)

  local life = self:get_life()
  if life <= 0 then
    local sprite = self:get_sprite()
    sprite:set_ignore_suspend(true)
    self:get_map():start_dialog("dungeon_3.arbror_killed")
    for _, t in ipairs(timers) do t:stop() end
    remove_sons()
  else
    if life > 9 then 
      nb_sons_immobilized_needed = 3
    elseif life > 7 then
      nb_sons_immobilized_needed = 4
    elseif life > 5 then
      nb_sons_immobilized_needed = 5
    else
      nb_sons_immobilized_needed = 6
    end
  end
end

function enemy:prepare_son()

  local sprite = self:get_sprite()
  if not vulnerable and sprite:get_animation() == "walking" then
    son_prefix = self:get_name() .. "_son"
    if self:get_map():enemy_get_group_count(son_prefix) < nb_sons_immobilized_needed then
      local sprite = self:get_sprite()
      sprite:set_animation("preparing_son")
      sol.audio.play_sound("hero_pushes")
      timers[#timers + 1] = sol.timer.start(1000, function() self:create_son() end)
      self:stop_movement()
    end
  end

  timers[#timers + 1] = sol.timer.start(math.random(2000, 5000), function() self:prepare_son() end)
end

function enemy:create_son()

  x = math.random(-7, 7) * 16

  nb_sons_created = nb_sons_created + 1
  son_name = self:get_name().."_son_"..nb_sons_created
  self:create_son(son_name, "arbror_root", x, 80)
  local speed = 48 + (initial_life - self:get_life()) * 5
  self:send_message(son_name, speed)
  sol.audio.play_sound("stone")
end

function enemy:on_sprite_animation_finished(sprite, animation)

  if animation == "preparing_son" then
    sprite:set_animation("walking")
    self:restart()
  elseif animation == "son_immobilized" then

    if nb_sons_immobilized >= nb_sons_immobilized_needed
        and not vulnerable then

      vulnerable = true
      self:set_attack_consequence("sword", 1)
      self:set_attack_consequence("arrow", 2)
      self:stop_movement()
      sprite:set_animation("vulnerable")
      sol.audio.play_sound("boss_hurt")
      for _, t in ipairs(timers) do t:stop() end
      timers[#timers + 1] = sol.timer.start(4000, function() self:stop_vulnerable() end)
      self:remove_sons()
    else
      sprite:set_animation("walking")
    end
  end
end

function enemy:on_message_received(src_enemy, message)

  if message == "begin immobilized" then
    if nb_sons_immobilized < nb_sons_immobilized_needed then
      nb_sons_immobilized = nb_sons_immobilized + 1
      local sprite = self:get_sprite()
      local animation = sprite:get_animation()

      if animation == "preparing_son" then
        self:restart()
      end

      sprite:set_animation("son_immobilized")
    end

  elseif message == "end immobilized" then
    if nb_sons_immobilized > 0 then
      nb_sons_immobilized = nb_sons_immobilized - 1
    end
  end
end

function enemy:stop_vulnerable()

  vulnerable = false
  self:remove_sons()
  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:restart()
end

function enemy:remove_sons()
 
  local son_prefix = self:get_name().."_son"
  --self:get_map():enemy_remove_group(son_prefix) 
  nb_sons_immobilized = 0

  for i = 1, nb_sons_created do
    son = son_prefix.."_"..i
    if not self:get_map():enemy_is_dead(son) then
      self:send_message(son, "disappear")
    end
  end
end


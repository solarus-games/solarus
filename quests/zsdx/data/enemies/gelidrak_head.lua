local enemy = ...

-- Gelidrak's head

local vulnerable = false      -- becomes vulnerable when the tail is hurt
local vulnerable_delay = 5000 -- delay while the head remains vulnerable
local nb_flames_created = 0
local timers = {}

function enemy:on_appear()

  self:set_life(24)
  self:set_damage(12)
  self:create_sprite("enemies/gelidrak_head")
  self:set_size(32, 40)
  self:set_origin(16, 24)
  self:set_hurt_style("boss")
  self:set_obstacle_behavior("flying")
  self:set_no_treasure()
  self:set_layer_independent_collisions(true)
  self:set_push_hero_on_sword(true)

  self:set_invincible()
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_attack_consequence("arrow", "protected")
  self:set_pushed_back_when_hurt(false)
end

function enemy:on_restart()

  if not vulnerable then
    for _, t in ipairs(timers) do t:stop() end
    timers[#timers + 1] = sol.timer.start(math.random(2000, 5000), function()
      self:throw_flames()
    end)
    self:go_back()
  else
    self:set_can_attack(false)
  end
end

function enemy:on_collision_enemy(other_name, other_sprite, my_sprite)

  if not vulnerable then
    self:go_back()
  end
end

function enemy:go_back()

  local x, y = self:get_map():enemy_get_position(self:get_father())
  local m = sol.movement.create("target")
  m:set_speed(16)
  m:set_target(x, y + 48)
  m:set_ignore_obstacles(true)
  self:start_movement(m)
end

function enemy:on_movement_finished(movement)

  local m = sol.movement.create("random")
  m:set_speed(16)
  m:set_max_distance(16)
  m:set_ignore_obstacles(true)
  self:start_movement(m)
  timers[#timers + 1] = sol.timer.start(5000, function() self:go_back() end)
end

function enemy:on_message_received(src_enemy, message)

  if src_enemy == self:get_father() then
    if message == "vulnerable" and not vulnerable then
      -- the head now becomes vulnerable
      vulnerable = true
      self:stop_movement()
      self:set_can_attack(false)
      self:set_attack_consequence("sword", 1)
      local sprite = self:get_sprite()
      sprite:set_animation("walking")
      for _, t in ipairs(timers) do t:stop() end
      timers[#timers + 1] = sol.timer.start(vulnerable_delay, function()
	vulnerable = false
	self:on_restart()
	self:set_can_attack(true)
        self:set_attack_consequence("sword", "protected")
	self:send_message(self:get_father(), "recovered")
      end)
    end
  end
end

function enemy:on_hurt(attack, life_lost)

  if self:get_life() > 0 then
    -- notify the body (so that it is hurt too)
    self:send_message(self:get_father(), "hurt")
  else
    for _, t in ipairs(timers) do t:stop() end
    self:send_message(self:get_father(), "dying")
  end
end

function enemy:on_dead()

  -- notify the body
  self:send_message(self:get_father(), "dead")
end

function enemy:throw_flames()

  if self:get_map():enemy_get_group_count(self:get_name() .. "_son_") < 5 then
    nb_flames_created = 0
    self:stop_movement()
    local sprite = self:get_sprite()
    sprite:set_animation("preparing_flame")
    sol.audio.play_sound("lamp")
    timers[#timers + 1] = sol.timer.start(500, function() self:repeat_flame() end)
  end
end

function enemy:repeat_flame()

  local max_flames_created = 32 - self:get_life()
  if nb_flames_created <= max_flames_created then
    nb_flames_created = nb_flames_created + 1
    local son_name = self:get_name() .. "_son_" .. nb_flames_created
    local angle = math.random(360) * math.pi / 180
    self:create_son(son_name, "blue_flame", 0, 16)
    self:send_message(son_name, tostring(angle))
    sol.audio.play_sound("lamp")
    timers[#timers + 1] = sol.timer.start(150, function() self:repeat_flame() end)
  else
    timers[#timers + 1] = sol.timer.start(500, function() self:restart() end)
  end
end


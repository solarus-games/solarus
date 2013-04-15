local enemy = ...

-- Billy

local going_hero = false

function enemy:on_created()

  self:set_life(30)
  self:set_damage(16)
  self:create_sprite("enemies/billy")
  self:set_hurt_style("boss")
  self:set_pushed_back_when_hurt(true)
  self:set_push_hero_on_sword(true)
  self:set_size(16, 16)
  self:set_origin(8, 13)
  self:set_invincible()
  self:set_attack_consequence("sword", 1)
end

function enemy:on_movement_changed()

  local m = self:get_movement()
  local direction4 = m:get_direction4()
  local sprite = self:get_sprite()
  sprite:set_direction(direction4)
end

function enemy:on_obstacle_reached()

  if not going_hero then
    self:go_random()
    self:check_hero()
  end
end

function enemy:on_restarted()
  self:go_random()
  self:check_hero()
end

function enemy:check_hero()

  local near_hero = self:get_distance(self:get_map():get_entity("hero")) < 100
  if near_hero and not going_hero then
    self:go_hero()
  elseif not near_hero and going_hero then
    self:go_random()
  end
  sol.timer.start(self, 1000, function() self:check_hero() end)
end

function enemy:go_random()
  local m = sol.movement.create("random_path")
  m:set_speed(48)
  m:start(self)
  going_hero = false
end

function enemy:go_hero()
  local m = sol.movement.create("target")
  m:set_speed(72)
  m:start(self)
  going_hero = true
end


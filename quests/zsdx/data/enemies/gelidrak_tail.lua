local enemy = ...

-- Gelidrak's tail.

local body = nil              -- Gelidrak's body.
local retracted = false       -- becomes retracted when touched by an arrow
local retracted_delay = 3000  -- delay while the tail remains retracted

function enemy:on_created()

  self:set_life(1000000)
  self:set_damage(12)
  self:create_sprite("enemies/gelidrak_tail")
  self:set_size(16, 16)
  self:set_origin(8, 8)
  self:set_hurt_style("boss")
  self:set_obstacle_behavior("flying")
  self:set_no_treasure()
  self:set_layer_independent_collisions(true)
  self:set_push_hero_on_sword(true)

  self:set_invincible()
  self:set_attack_consequence("arrow", 1)
  self:set_attack_consequence("sword", "protected")
  self:set_attack_consequence("hookshot", "protected")
  self:set_attack_consequence("boomerang", "protected")
  self:set_pushed_back_when_hurt(false)

  -- Go to the high layer.
  local x, y = self:get_position()
  self:set_position(x, y, 2)
end

function enemy:on_restarted()

  if not retracted then
    local m = sol.movement.create("path")
    m:set_path{0,0,0,0,7,7,3,3,4,4,4,4,4,4,4,4,5,5,1,1,0,0,0,0}
    m:set_speed(64)
    m:set_ignore_obstacles(true)
    m:set_loop(true)
    m:start(self)
  else
    self:stop_movement()
    sol.timer.start(self, retracted_delay, function()
      local x, y = body:get_position()
      local m = sol.movement.create("target")
      m:set_speed(48)
      m:set_target(x, y - 112)
      m:start(self)
    end)
  end
end

function enemy:on_movement_finished()

  -- The tail was retracted and has just recovered its normal position.
  retracted = false
  self:set_attack_consequence("arrow", 1)
  self:set_can_attack(true)
  self:on_restarted()

  if body:exists() then
    body:tail_recovered()
  end
end

function enemy:on_hurt(attack, life_lost)

  -- Notify the body so that it stops moving and becomes vulnerable.
  body:tail_hurt()

  -- Retract the tail for a while.
  retracted = true
  local x, y = body:get_position()
  self:set_position(x, y - 48)
  self:set_attack_consequence("arrow", "protected")
  self:set_can_attack(false)
  -- TODO remove the arrow?
end


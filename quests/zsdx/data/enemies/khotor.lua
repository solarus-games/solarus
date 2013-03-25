local enemy = ...

-- The boss Khotor. He controls a chain and ball.

local chain = nil  -- The chain and ball created.

function enemy:on_created()

  self:set_life(12)
  self:set_damage(3)
  self:set_pushed_back_when_hurt(false)
  self:set_push_hero_on_sword(true)
  self:create_sprite("enemies/khotor")
  self:set_size(48, 48)
  self:set_origin(24, 29)
  self:set_invincible()
  self:set_attack_consequence("sword", 2)
  self:set_attack_consequence("thrown_item", 1)
  self:set_attack_consequence("arrow", 1)

  -- Create the chain and ball.
  local chain_name = self:get_name() .. "_chain"
  chain = self:create_enemy(chain_name, "chain_and_ball", -16, -33)
  chain:set_center_enemy(self)
end

function enemy:on_restarted()

  -- Set the movement.
  local m = sol.movement.create("random_path")
  m:set_speed(40)
  m:start(self)

  chain:set_enabled(true)
end

function enemy:on_hurt(attack, life_lost)

  if self:get_life() <= 0 then
    -- Khotor is dying: remove the chain and ball.
    chain:remove()
  elseif (life_lost > 0) then
    -- Khotor is hurt: disable the chain and ball for a while.
    chain:set_enabled(false)
  end
end


local enemy = ...

-- The boss Khotor. He controls a chain and ball.

local chain_name = ""

function enemy:on_appear(creator_name)

  -- set the properties
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

  -- create the chain and ball
  chain_name = self:get_name().."_chain"
  self:create_son(chain_name, "chain_and_ball", -16, -33)
end

function enemy:on_restart()

  -- set the movement
  local m = sol.movement.create("random_path")
  m:set_speed(40)
  self:start_movement(m)

  self:get_map():enemy_set_enabled(chain_name, true)
end

function enemy:on_hurt(attack, life_lost)

  if self:get_life() <= 0 then

    -- Khotor is dying: remove the chain and ball
    self:get_map():enemy_remove(chain_name)
  elseif (life_lost > 0) then

    -- Khotor is hurt: restart the chain and ball
    self:get_map():enemy_set_enabled(chain_name, false)
  end
end


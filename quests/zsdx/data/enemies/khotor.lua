-- The boss Khotor. He controls a chain and ball.

chain_name = ""

function event_appear(creator_name)

  -- set the properties
  sol.enemy.set_life(12)
  sol.enemy.set_damage(2)
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.create_sprite("enemies/khotor")
  sol.enemy.set_size(48, 48)
  sol.enemy.set_origin(24, 29)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", 2)
  sol.enemy.set_attack_consequence("thrown_item", 1)
  sol.enemy.set_attack_consequence("arrow", 1)

  -- set the movement
  m = sol.main.random_path_movement_create(40)
  sol.enemy.set_movement(m)

  -- create the chain and ball
  chain_name = sol.enemy.get_name().."_chain"
  sol.enemy.create_son(chain_name, "chain_and_ball", layer, -16, -33, true)
end

function event_restart()

  sol.map.enemy_set_enabled(chain_name, true)
end

function event_hurt(attack, life_lost)

  if sol.enemy.get_life() <= 0 then

    -- Khotor is dying: remove the chain and ball
    sol.map.enemy_remove(chain_name)
  elseif (life_lost > 0) then

    -- Khotor is hurt: restart the chain and ball
    sol.map.enemy_set_enabled(chain_name, false);
  end
end


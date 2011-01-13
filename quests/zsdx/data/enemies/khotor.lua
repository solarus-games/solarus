-- The boss Khotor. He controls a chain and ball.

-- Properties
life = 12
damage = 2
pushed_back_when_hurt = false
sprite = "enemies/khotor"
size = {48, 48}
origin = {24, 29}
attack_consequences = {
  sword = 2,
  thrown_item = 1,
  bow = 1,
  others = "ignored"
}

chain_name = ""

function event_appear(creator_name)

  -- set the movement
  m = sol.main.random_path_movement_create(40)
  sol.enemy.set_movement(m)

  -- create the chain and ball
  chain_name = sol.enemy.get_name().."_chain"
  sol.map.enemy_create(chain_name, "chain_and_ball", layer, x, y)
  sol.map.enemy_set_creator(chain_name, sol.enemy.get_name(), -16, -33)
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


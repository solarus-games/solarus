-- Black knight soldier (captures the hero)

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/black_knight_soldier")
  sol.enemy.create_sprite("enemies/black_knight_soldier_sword")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
end

function event_restart()

end


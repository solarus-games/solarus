-- Pike that does not move

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/pike_fixed")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_can_hurt_hero_running(true)
  sol.enemy.set_invincible()
end


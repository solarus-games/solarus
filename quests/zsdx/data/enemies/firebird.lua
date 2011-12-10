-- Firebird

function event_appear()

  sol.enemy.set_life(10)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/firebird")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_layer_independent_collisions(true)

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_pushed_back_when_hurt(false)
  sol.enemy.set_push_hero_on_sword(true)
end

function event_restart()

end


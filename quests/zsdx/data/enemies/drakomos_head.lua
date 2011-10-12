-- Drakomos head

function event_appear()

  sol.enemy.set_life(24)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/drakomos_head")
  sol.enemy.set_size(56, 56)
  sol.enemy.set_origin(28, 40)
  sol.enemy.set_hurt_sound_style("boss")

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("thrown_item", 1)
end

function event_restart()

  local m = sol.main.random_movement_create(48)
  sol.main.movement_set_property(m, "max_distance", 24)
  sol.main.movement_set_property(m, "ignore_obstacles", true)
  sol.enemy.start_movement(m)
end


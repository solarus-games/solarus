-- A root of Master Arbror

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/arbror_root")
  sol.enemy.set_size(64, 96)
  sol.enemy.set_origin(28, 86)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("hookshot", "immobilized")
end


eye = nil

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(6)
  sol.enemy.create_sprite("enemies/flora_gohma")
  sol.enemy.set_size(80, 80)
  sol.enemy.set_origin(40, 64)
  sol.enemy.set_invincible()

  -- create the eye
  local my_name = sol.enemy.get_name()
  eye = my_name.."_eye"
  sol.enemy.create_son(eye, "flora_gohma_eye", 0, -80)
end


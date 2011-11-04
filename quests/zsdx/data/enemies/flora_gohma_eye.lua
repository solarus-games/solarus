petals = {}

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/flora_gohma_eye")
  sol.enemy.set_size(104, 64)
  sol.enemy.set_origin(52, 64)
  sol.enemy.set_invincible()

  for i = 1, 5 do
    petals[i] = sol.enemy.create_sprite("enemies/flora_gohma_eye")
    sol.main.sprite_set_animation(petals[i], "petal_"..i)
    sol.enemy.set_attack_consequence_sprite(petals[i], "hookshot", "custom")
  end
end

function event_custom_attack_received(attack, sprite)

  -- a petal was touched by the hookshot
  print("touched sprite", sprite)
  for i = 1, 5 do
    if petals[i] == sprite then
      sol.enemy.remove_sprite(petals[i])
      petals[i] = nil
    end
  end
  return 0
end


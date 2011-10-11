function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/evil_tile")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "custom")
end

function event_restart()

end

function event_custom_attack_received(attack, sprite)

  sol.enemy.set_invincible()
  sol.enemy.set_can_attack(false)
  sol.main.sprite_set_animation(sprite, "destroy")
  sol.main.play_sound("stone")
end

function event_sprite_animation_finished(sprite, animation)

  if animation == "destroy" then
    sol.map.enemy_remove(sol.enemy.get_name())
  end
end


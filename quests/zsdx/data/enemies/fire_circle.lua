-- A circle of fire

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(1)
  sol.enemy.create_sprite("enemies/fire_circle")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
end

function event_restart()

  -- disappear after a few seconds
  sol.main.timer_start(function()
    sol.map.enemy_remove(sol.enemy.get_name())
  end, 3000)
end

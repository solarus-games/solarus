function event_appear()

  if sol.enemy.test_obstacles(0, 0) then
    -- don't appear on stones previously created
    sol.map.enemy_remove(sol.enemy.get_name())
  end

  sol.enemy.create_sprite("enemies/drakomos_lava_spawner")
  sol.enemy.set_size(32, 24)
  sol.enemy.set_origin(16, 21)
  sol.enemy.set_invincible()
end

function event_restart()

  sol.enemy.set_can_attack(false)
  sol.main.timer_start(function()
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "disappearing")
    sol.main.play_sound("ice")

    if math.random(2) == 1 or sol.enemy.get_distance_to_hero() < 24 then
      local son_name = sol.enemy.get_name().."_son"
      sol.enemy.create_son(son_name, "red_helmasaur", 0, 0)
      if sol.game.get_life() <= sol.game.get_max_life() / 3 then
        sol.map.enemy_set_treasure(son_name, "heart", 1, -1)
      end
    else
      local x, y = sol.enemy.get_position()
      local layer = sol.enemy.get_layer()
      sol.map.destructible_item_create(4, x, y, layer)
    end
  end, 1000)
end

function event_sprite_animation_finished(sprite, animation)

  if animation == "disappearing" then
    sol.map.enemy_set_enabled(sol.enemy.get_name(), false)
  end
end


shadow_sprite = nil
initial_y = nil
state = nil -- "raising", "attacking", "destroying"

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/evil_tile")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "custom")

  shadow_sprite = sol.main.sprite_create("entities/shadow")
  sol.main.sprite_set_animation(shadow_sprite, "big")
end

function event_restart()

  local x, y = sol.enemy.get_position()
  initial_y = y

  local m = sol.main.path_movement_create("22", 16)
  sol.enemy.start_movement(m)
  sol.main.timer_start(go_hero, 2000)
  state = "raising"
end

function go_hero()

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  local angle = sol.main.get_angle(x, y, hero_x, hero_y)
  local m = sol.main.straight_movement_create(192, angle)
  sol.enemy.start_movement(m)
  state = "attacking"
end

function event_obstacle_reached()
  destroy()
end

function event_custom_attack_received(attack, sprite)

  if state == "attacking" then
    destroy()
  end
end

function destroy()

  if state ~= "destroying" then
    state = "destroying"
    local sprite = sol.enemy.get_sprite()
    sol.enemy.set_can_attack(false)
    sol.enemy.stop_movement()
    sol.main.sprite_set_animation(sprite, "destroy")
    sol.main.play_sound("stone")
    sol.main.timer_stop_all()
  end
end

function event_sprite_animation_finished(sprite, animation)

  if state == "destroying" then
    sol.map.enemy_remove(sol.enemy.get_name())
  end
end

function event_pre_display()

  -- show the shadow
  if state ~= "destroying" then
    local x, y = sol.enemy.get_position()
    if state == "attacking" then
      y = y + 16
    else
      y = initial_y
    end
    sol.map.sprite_display(shadow_sprite, x, y)
  end
end


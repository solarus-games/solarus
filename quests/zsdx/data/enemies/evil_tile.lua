local shadow_sprite = nil
local initial_y = nil
local state = nil -- "raising", "attacking", "destroying"
local timer

function event_appear()

  sol.enemy.set_life(1)
  sol.enemy.set_damage(4)
  sol.enemy.create_sprite("enemies/evil_tile")
  sol.enemy.set_size(16, 16)
  sol.enemy.set_origin(8, 13)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "custom")

  shadow_sprite = sol.sprite.create("entities/shadow")
  shadow_sprite:set_animation("big")
end

function event_restart()

  local x, y = sol.enemy.get_position()
  initial_y = y

  local m = sol.movement.create("path")
  m:set_path{2,2}
  m:set_speed(16)
  sol.enemy.start_movement(m)
  timer = sol.main:start_timer(2000, go_hero)
  state = "raising"
end

function go_hero()

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  local angle = sol.main.get_angle(x, y, hero_x, hero_y)
  local m = sol.movement.create("straight")
  m:set_speed(192)
  m:set_angle(angle)
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
    sprite:set_animation("destroy")
    sol.audio.play_sound("stone")
    if timer ~= nil then
      timer:stop()
    end
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


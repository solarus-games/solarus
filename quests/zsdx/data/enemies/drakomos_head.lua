-- Drakomos head

nb_fire_created = 0
max_fire_created = 10
initial_xy = {}

function event_appear()

  sol.enemy.set_life(16)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/drakomos_head")
  sol.enemy.set_size(56, 56)
  sol.enemy.set_origin(28, 40)
  sol.enemy.set_hurt_sound_style("boss")
  sol.enemy.set_obstacle_behavior("flying")
  sol.enemy.set_no_treasure()

  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("sword", "protected")
  sol.enemy.set_attack_consequence("hookshot", "protected")
  sol.enemy.set_attack_consequence("boomerang", "protected")
  sol.enemy.set_attack_consequence("arrow", "protected")
  sol.enemy.set_attack_consequence("thrown_item", 1)

  initial_xy.x, initial_xy.y = sol.enemy.get_position()
end

function event_restart()

  local m = sol.main.target_movement_create(48, initial_xy.x, initial_xy.y)
  sol.enemy.start_movement(m)
  sol.main.timer_stop_all()

  nb_fire_created = 0
  sol.main.timer_start(function()
      sol.enemy.stop_movement()
      local sprite = sol.enemy.get_sprite()
      sol.main.sprite_set_animation(sprite, "preparing_fire")
      sol.main.play_sound("lamp")
      sol.main.timer_start(repeat_fire, 500)
    end,
    2000 + math.random(8000))
end

function event_hurt(attack, life_lost)

  if life_lost > 0 then
    sol.main.timer_stop_all()
  end
end

function event_movement_finished(movement)

  local m = sol.main.random_movement_create(48)
  sol.main.movement_set_property(m, "max_distance", 24)
  sol.enemy.start_movement(m)
end

function repeat_fire()

  if nb_fire_created < max_fire_created then
    nb_fire_created = nb_fire_created + 1
    local son_name = sol.enemy.get_name().."_son_"..nb_fire_created
    local angle_start = 2 * math.pi / 4
    local angle_end = 9 * math.pi / 4
    local angle = angle_start + nb_fire_created * (angle_end - angle_start) / max_fire_created
    sol.enemy.create_son(son_name, "fireball_simple", 0, 16)
    sol.enemy.send_message(son_name, tostring(angle))
    sol.main.play_sound("lamp")
    sol.main.timer_start(repeat_fire, 150)
  else
    sol.main.timer_start(sol.enemy.restart, 500)
  end
end


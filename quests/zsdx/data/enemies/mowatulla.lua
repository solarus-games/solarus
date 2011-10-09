-- A spider boss from Newlink

nb_sons_to_create = 0
nb_sons_created = 0

function event_appear()

  sol.enemy.set_life(8)
  sol.enemy.set_damage(2)
  sol.enemy.create_sprite("enemies/mowatulla")
  sol.enemy.set_size(96, 112)
  sol.enemy.set_origin(48, 64)
  sol.enemy.set_invincible()
  sol.enemy.set_attack_consequence("explosion", 1)
end

function event_restart()

  sol.main.timer_start(jump_or_son_phase, 500)
  go()
end

function event_movement_finished(movement)

  event_restart()
end

function event_update()

  local x, y = sol.enemy.get_position()
  local hero_x, hero_y = sol.map.hero_get_position()
  if hero_y > y + 8 then
    sol.enemy.set_attack_consequence("sword", 1)
    sol.enemy.set_attack_consequence("arrow", 2)
  else
    sol.enemy.set_attack_consequence("sword", "protected")
    sol.enemy.set_attack_consequence("arrow", "protected")
  end
end

function event_hurt(attack, life_lost)

  sol.main.timer_stop_all()
  if sol.enemy.get_life() <= 0 then
    -- I am dying: remove the sons
    local sons_prefix = sol.enemy.get_name().."_son"
    sol.map.enemy_remove_group(sons_prefix)
  end
end

function go()
  local m = sol.main.random_path_movement_create(64)
  sol.enemy.start_movement(m)
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "walking")
end

function son_phase_soon()

  sol.enemy.stop_movement()
  sol.main.timer_start(son_phase, 500)
end

function son_phase()

  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "preparing_son")
  sol.main.play_sound("boss_charge")
  sol.main.timer_start(throw_son, 1500)

  if sol.enemy.get_life() < 3 then
    nb_sons_to_create = 3
  else
    nb_sons_to_create = 1
  end

end

function throw_son()

  -- create the son
  nb_sons_created = nb_sons_created + 1
  local son_name = sol.enemy.get_name().."_minillosaur_"..nb_sons_created
  sol.enemy.create_son(son_name, "mini_mowatulla", 0, 40)
  sol.main.play_sound("boss_fireball")

  -- see what to do next
  nb_sons_to_create = nb_sons_to_create - 1
  if nb_sons_to_create > 0 then
    -- throw another son in 0.5 second
    sol.main.timer_start(throw_son, 500)
  else
    -- finish the son phase
    local sprite = sol.enemy.get_sprite()
    sol.main.sprite_set_animation(sprite, "walking")
    local duration = 3500 + (math.random(3) * 1000)
    sol.main.timer_start(jump_or_son_phase, duration)
    sol.main.timer_start(go, 500)
  end
end

function jump_or_son_phase()

  if math.random(2) == 1 then
 
    local sons_prefix = sol.enemy.get_name().."_son" 
    local nb_sons = sol.map.enemy_get_group_count(sons_prefix)
    if nb_sons < 5 then
      son_phase_soon()
    else
      -- no son phase if there are already too much sons: jump instead
      jump_phase()
    end
  else
    jump_phase()
  end
end

function jump_phase()

  local x, y = sol.enemy.get_position()
  y = y - 16
  local hero_x, hero_y = sol.map.hero_get_position()
  if hero_x > x then
    if y > 856 then 
      direction8 = 1
    else
      direction8 = 0
    end
  else
    if y > 856 then 
      direction8 = 3
    else
      direction8 = 4
    end
  end

  local m = sol.main.jump_movement_create(direction8, 112)
  sol.main.movement_set_property(m, "speed", 96)
  sol.enemy.start_movement(m)
  local sprite = sol.enemy.get_sprite()
  sol.main.sprite_set_animation(sprite, "jumping")
  sol.main.play_sound("jump")
end

